// SillikOne.


#include "NPC_Character.h"
#include "IC/NPC/NPC_AiController.h"
#include "BehaviorTree/BlackboardComponent.h"
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
	return CharacterStatComponent->SpeedCurrent;
}

void ANPC_Character::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AICCharacter* Player = Cast<AICCharacter>(OtherActor);

	ANPC_AiController* AIController = Cast<ANPC_AiController>(GetController());
	if (!AIController) { return; }

	if (Player && AIController->bCanSeePlayer && !Player->bIsInCombat)
	{
		// Player Entered trigger zone and AI can see player, player not already in combat
		UEncounterSytemComponent* EncounterComponent = Player->FindComponentByClass<UEncounterSytemComponent>();
		EncounterComponent->bPlayerHasInitiative = false;
	
		GatherNpc(Player);
		Player->EncounterComponent->StartEncounter();
	}
	if (Player && !AIController->bCanSeePlayer && !Player->bIsInCombat)
	{
		// if Player enters encounter zone but is not seen nor already in combat.
		UEncounterSytemComponent* EncounterComponent = Player->FindComponentByClass<UEncounterSytemComponent>();
		EncounterComponent->bPlayerHasInitiative = true;

		// TODO Surprise attack
	}
	// New npc comes across during combat
	ANPC_Character* NewComer = Cast<ANPC_Character>(OtherActor);
	if (NewComer && bIsInCombat && !NewComer->bIsInCombat)
	{
		GatherNpc(CurrentPlayer);
	}
}

void ANPC_Character::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AICCharacter* Player = Cast<AICCharacter>(OtherActor);
	if (Player)
	{
		UEncounterSytemComponent* EncounterComponent = Player->FindComponentByClass<UEncounterSytemComponent>();
		EncounterComponent->bPlayerHasInitiative = false;
	}
}

void ANPC_Character::GatherNpc(AICCharacter* Player)
{
	// Checking if there are some npc around
	TArray <AActor*> OverlappingActors;
	EncounterTrigger->GetOverlappingActors(OverlappingActors);

	CurrentPlayer = Cast<AICCharacter>(Player);

	for (int32 i = 0; i < OverlappingActors.Num(); i++)
	{
		ANPC_Character* NpcToAdd = Cast<ANPC_Character>(OverlappingActors[i]);
		if (NpcToAdd)
		{
			// Make an array of npcs who will join
			CurrentPlayer->NpcEncounter.AddUnique(NpcToAdd);
			ANPC_AiController* AIController = Cast<ANPC_AiController>(NpcToAdd->GetController());
			AIController->BlackboardComponent->SetValueAsObject("Player", Player);
			NpcToAdd->bIsInCombat = true;
		}
	}
}

