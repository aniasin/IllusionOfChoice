// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "NPC_AiController.generated.h"

/**
 * 
 */
UCLASS()
class IC_API ANPC_AiController : public AAIController
{
	GENERATED_BODY()

	ANPC_AiController(const FObjectInitializer& ObjectInitializer);

	class ANPC_Character* AICharacter;

	class UAIPerceptionComponent* PerceptionComponent;
	class UAISenseConfig_Sight* SightConfig;
	class UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(EditDefaultsOnly, Category = NPC_AI)
		float SightRange = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = NPC_AI)
		float LoseSightRange = 2500.f;
	UPROPERTY(EditDefaultsOnly, Category = NPC_AI)
		float HearingRange = 1000.f;
	UPROPERTY(EditAnywhere, Category = NPC_AI)
		class UBehaviorTree* BehaviorTree;
	UBlackboardComponent* BlackboardComponent;

		UFUNCTION()
		void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPC_AI)
		bool bCanSeeActor = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPC_AI)
		bool bCanSeePlayer = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPC_AI)
		FVector LastKnownPlayerPosition;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPC_AI)
		FVector LastKnownPlayerDirection;

protected:
	void OnPossess(APawn* InPawn);
};
