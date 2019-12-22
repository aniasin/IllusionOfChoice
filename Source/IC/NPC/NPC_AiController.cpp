// SillikOne.


#include "NPC_AiController.h"
#include "NPC_Character.h"
#include "IC/ICCharacter.h"
#include "IC/Combat/EncounterSytemComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionTypes.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GenericTeamAgentInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

ANPC_AiController::ANPC_AiController()
{
	// Creating Perception component
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(FName("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(FName("HearingConfig"));

	// Configure the sight and hearing sense
	SightConfig->SightRadius = SightRange;
	SightConfig->LoseSightRadius = LoseSightRange;
	HearingConfig->HearingRange = HearingRange;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	// Assign the sight and hearing sense to the perception component
	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*HearingConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ANPC_AiController::OnTargetPerceptionUpdate);

	// Assign this controller to team 10
	SetGenericTeamId(FGenericTeamId(10));
}


void ANPC_AiController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AICharacter = Cast<ANPC_Character>(InPawn);
}

void ANPC_AiController::OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	if (!AICharacter) { return; }

	//Retrieving perceived actors
	TArray<AActor*> SeenActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(TSubclassOf<UAISense_Sight>(), SeenActors);

	// Numbers of seen actors and if they enter or exit view
	int32 NumberOfActorsSeen = SeenActors.Num();

	LastKnownPlayerPosition = Stimulus.StimulusLocation;
	bCanSeeActor = Stimulus.WasSuccessfullySensed();

	AICCharacter* Player = Cast<AICCharacter>(Actor);

	// Gain sight with Player
	if (bCanSeeActor && Player)
	{
		bCanSeePlayer = true;
		UE_LOG(LogTemp, Warning, TEXT("%s : Gain sight with Player"), *AICharacter->GetName())

		if (Player->EncounterComponent->bPlayerHasInitiative)
		{
			AICharacter->GatherNpcAndStartEncounter(Player);
		}
	}

	// Sight is lost with Player
	if (!bCanSeeActor && Player)
	{
		bCanSeePlayer = false;
		UE_LOG(LogTemp, Warning, TEXT("%s : Loose ight with Player"), *AICharacter->GetName())
	}
}
