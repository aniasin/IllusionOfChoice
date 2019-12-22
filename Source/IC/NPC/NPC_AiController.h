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
		float SightRange = 4000.f;
	UPROPERTY(EditDefaultsOnly, Category = NPC_AI)
		float LoseSightRange = 4500.f;
	UPROPERTY(EditDefaultsOnly, Category = NPC_AI)
		float HearingRange = 1000.f;
	UPROPERTY(EditAnywhere, Category = NPC_AI)
		class UBehaviorTree* BehaviorTree;

		UFUNCTION()
		void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

public:
	UPROPERTY()
		UBlackboardComponent* BlackboardComponent;
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
