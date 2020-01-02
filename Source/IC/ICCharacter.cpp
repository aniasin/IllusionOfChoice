// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ICCharacter.h"
#include "IC/ICGameInstance.h"
#include "IC/UI/EncounterPanel.h"
#include "GameFramework/PlayerController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "IC/Characters/CharacterStatComponent.h"
#include "IC/Combat/EncounterSytemComponent.h"
#include "IC/Combat/InventoryComponent.h"
#include "Camera/CameraActor.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "IC/Weapons/Weapon_Base.h"
#include "DrawDebugHelpers.h"

//////////////////////////////////////////////////////////////////////////
// AICCharacter

AICCharacter::AICCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	CharacterStatComponent = CreateDefaultSubobject<UCharacterStatComponent>(FName("CharacterStatComponent"));
	EncounterComponent = CreateDefaultSubobject<UEncounterSytemComponent>(FName("EncounterComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(FName("InventoryComponent"));

	CursorComponent = CreateDefaultSubobject<USphereComponent>(FName("Cursor3D"));
	CursorComponent->SetSphereRadius(64, true);
	CursorComponent->SetupAttachment(RootComponent);
	Cursor3DDecal = CreateDefaultSubobject<UDecalComponent>(FName("CursorDecal"));
	Cursor3DDecal->SetupAttachment(CursorComponent);
	Cursor3DDecal->SetVisibility(false);
	Cursor3DDecal->DecalSize = FVector(32, 64, 64);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AICCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Click", IE_Pressed, this, &AICCharacter::Click);
	PlayerInputComponent->BindAction("Click", IE_Released, this, &AICCharacter::StopClick);

	PlayerInputComponent->BindAxis("MoveForward", this, &AICCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AICCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AICCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AICCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AICCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AICCharacter::TouchStopped);
}

void AICCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initialize PlayerTeam array with player...
	PlayerTeam.AddUnique(this);
	// ...and variables
	bWantToMove = false;
	bCanMove = true;
}

void AICCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PositionCursorToWorld();
}

float AICCharacter::GetWeaponReach()
{
	return InventoryComponent->RHand->GetDefaultObject<AWeapon_Base>()->Reach;
}

float AICCharacter::GetCurrentSpeed() {	return CharacterStatComponent->SpeedCurrent; }
float AICCharacter::GetCurrentHealth() { return CharacterStatComponent->HealthCurrent; }

void AICCharacter::Click()
{
	if (bWantToMove && bCanMove && LocationToMove != FVector(0,0,0) && NumberOfMove <= 2)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, LocationToMove);

		bWantToMove = false;
		EncounterPanel->ClosePanel();
		Cursor3DDecal->ToggleVisibility(false);
		CursorComponent->SetWorldLocationAndRotation(GetActorLocation(), GetActorRotation());
		NumberOfMove++;
		if (NumberOfMove >= 1)
		{
			EncounterComponent->IncrementTurnsAndRounds(true);
		}
	}
}

void AICCharacter::StopClick()
{

}

void AICCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AICCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AICCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AICCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AICCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && !bIsInCombat)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AICCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) && !bIsInCombat)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AICCharacter::PositionCursorToWorld()
{
	if (bWantToMove && bCanMove && NumberOfMove <= 2)
	{
		FHitResult HitResult;
		FVector TraceStart = FollowCamera->GetComponentLocation();
		float Distance = CharacterStatComponent->SpeedCurrent * 40 - GetWeaponReach();
		FVector TraceEnd = FollowCamera->GetForwardVector() * Distance + TraceStart;
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility);
		FVector Location = HitResult.Location;
		FVector Normal = HitResult.ImpactNormal;

		FRotator Rotation = UKismetMathLibrary::MakeRotationFromAxes(Normal, FVector(0, 0, 0), FVector(0, 0, 0));
		CursorComponent->SetWorldLocationAndRotation(Location, Rotation);
		if (HitResult.IsValidBlockingHit())
		{
			LocationToMove = Location;
		}
		else
		{
			LocationToMove = FVector(0, 0, 0);
		}
	}
}

void AICCharacter::CreateEncounterPanel()
{
	ToggleCameraBoom(true);
	UICGameInstance* GameInstance = Cast<UICGameInstance>(GetGameInstance());
	if (!GameInstance) { return; }
	EncounterPanel = GameInstance->LoadEncounterPanel(this);
}

void AICCharacter::ToggleCameraBoom(bool bEncounter)
{
	float Length;
	FVector Location;
	bEncounter ? Length = 600 : Length = 300;
	bEncounter ? Location = FVector(0, 0, 70) : Location = FVector(0, 0, 0);
	CameraBoom->TargetArmLength = Length;
	CameraBoom->SetRelativeLocation(Location);
}

