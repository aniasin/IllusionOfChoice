// SillikOne.


#include "NPC_Character.h"
#include "IC/NPC/NPC_AiController.h"
#include "IC/ICCharacter.h"
#include "IC/Combat/EncounterSytemComponent.h"
#include "IC/Characters/CharacterStatComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ANPC_Character::ANPC_Character(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CharacterStatComponent = CreateDefaultSubobject<UCharacterStatComponent>(FName("CharacterStatComponent"));

	EncounterTrigger = CreateDefaultSubobject<USphereComponent>(FName("EncounterTrigger"));
	EncounterTrigger->SetupAttachment(RootComponent);
	EncounterTrigger->SetSphereRadius(1536.f, true);
	EncounterTrigger->SetCollisionProfileName(TEXT("CombatZone"));
	EncounterTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	EncounterTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);


	EncounterTrigger->OnComponentBeginOverlap.AddDynamic(this, &ANPC_Character::OnOverlapBegin);
	EncounterTrigger->OnComponentEndOverlap.AddDynamic(this, &ANPC_Character::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ANPC_Character::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ANPC_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANPC_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ANPC_Character::GetCurrentSpeed()
{
	if (!CharacterStatComponent) { return 0.f; }
	return CharacterStatComponent->SpeedCurrent;
}

void ANPC_Character::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AICCharacter* Player = Cast<AICCharacter>(OtherActor);
	if (Player)
		// Player Entered trigger zone
	{
		if (Player->bIsInCombat) { return; }

		// Checking if there are some npc around
		TArray <AActor*> OverlappingActors;
		EncounterTrigger->GetOverlappingActors(OverlappingActors);

		for (int32 i = 0 ; i < OverlappingActors.Num() ; i++)
		{
			ANPC_Character* NpcToAdd = Cast<ANPC_Character>(OverlappingActors[i]);
			if (NpcToAdd)
			{
				// Make an array of npcs who will join
				Player->NpcEncounter.Add(NpcToAdd);
			}
		}

		// Checking if Player has initiative
		ANPC_AiController* AIController = Cast<ANPC_AiController>(GetController());
		if (!AIController) { return; }
		
		if (AIController->bCanSeePlayer)
		{
			// If AI can see Player, Player has not the initiative
			//TODO Set bool in COmbatComponent
		}
		else
		{
			// If AI can see Player, Player has the initiative
			// TODO Set the bool
		}

		// TODO start Encounter in CombatComponent
		Player->StartEncounter();
	}
}

void ANPC_Character::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

