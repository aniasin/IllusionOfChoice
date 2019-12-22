// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_ChoosePatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class IC_API UBTTaskNode_ChoosePatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()
	
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;

protected:

	UPROPERTY(VisibleAnywhere, Category = "Blackboard")
		FBlackboardKeySelector IndexKey;

	UPROPERTY(VisibleAnywhere, Category = "Blackboard")
		FBlackboardKeySelector WaypointKey;
	UPROPERTY(VisibleAnywhere, Category = "Blackboard")
		FBlackboardKeySelector PlayerKey;

private:

	class AAIController* AIController;
	class UBlackboardComponent* BlackboardComponent;
};
