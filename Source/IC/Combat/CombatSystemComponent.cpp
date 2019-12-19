// SillikOne.


#include "CombatSystemComponent.h"
#include "Containers/Array.h"
#include "IC/ICCharacter.h"
#include "IC/NPC/NPC_Character.h"

// Sets default values for this component's properties
UCombatSystemComponent::UCombatSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCombatSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<AICCharacter*> UCombatSystemComponent::SortPlayersBySpeed(TArray<AICCharacter*> PlayerToSort)
{
	auto SortPred = [](AICCharacter& A, AICCharacter& B)->bool
	{
		return(A.GetCurrentSpeed()) <= (B.GetCurrentSpeed());
	};
	PlayerToSort.Sort(SortPred);

	return PlayerToSort;
}

TArray<class ANPC_Character*> UCombatSystemComponent::SortNPCBySpeed(TArray<class ANPC_Character*> NPCToSort)
{
	auto SortPred = [](ANPC_Character& A, ANPC_Character& B)->bool
	{
		return(A.GetCurrentSpeed()) <= (B.GetCurrentSpeed());
	};
	NPCToSort.Sort(SortPred);

	return NPCToSort;
}
