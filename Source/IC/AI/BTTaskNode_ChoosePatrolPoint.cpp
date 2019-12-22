// SillikOne.


#include "BTTaskNode_ChoosePatrolPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "IC/NPC/NPC_AiController.h"
#include "IC/AI/PatrolComponent.h"

EBTNodeResult::Type UBTTaskNode_ChoosePatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get Blackboard
	BlackboardComponent = OwnerComp.GetBlackboardComponent();
	AIController = OwnerComp.GetAIOwner();
	if (!BlackboardComponent || !AIController) { return EBTNodeResult::Failed; }

	ANPC_AiController* NpcController = Cast<ANPC_AiController>(AIController);
	if (PlayerKey.IsSet())
	{
		return EBTNodeResult::Failed;
	}

	// Get Patrol Route stored in PatrolComponent
	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	UPatrolComponent* PatrolComponent = AIPawn->FindComponentByClass<UPatrolComponent>();
	if (!PatrolComponent) { return EBTNodeResult::Failed; }

	TArray<AActor*>PatrolRoute = PatrolComponent->GetPatrolRoute();
	if (PatrolRoute.Num() == 0) { return EBTNodeResult::Failed; }

	// Remember last patrol point
	BlackboardComponent->SetValueAsObject("PreviousPatrolPoint", BlackboardComponent->GetValueAsObject(WaypointKey.SelectedKeyName));

	// Set Next Patrol Point in BB
	int32 IndexValue = BlackboardComponent->GetValueAsInt(IndexKey.SelectedKeyName);
	BlackboardComponent->SetValueAsObject(WaypointKey.SelectedKeyName, PatrolRoute[IndexValue]);

	UE_LOG(LogTemp, Warning, TEXT("%s: Now going to patrol point %s"), *AIPawn->GetName(),
		*PatrolRoute[IndexValue]->GetName())

		// Cycle through Patrol Points
		int32 NewIndexValue = (IndexValue + 1) % PatrolRoute.Num();
	BlackboardComponent->SetValueAsInt(IndexKey.SelectedKeyName, NewIndexValue);

	return EBTNodeResult::Succeeded;
}
