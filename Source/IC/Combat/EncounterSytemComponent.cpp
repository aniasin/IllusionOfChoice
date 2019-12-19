// SillikOne.


#include "EncounterSytemComponent.h"
#include "Containers/Array.h"
#include "IC/ICCharacter.h"
#include "IC/NPC/NPC_Character.h"
#include "Blueprint/UserWidget.h"
#include "IC/UI/LogMessageWidget.h"

// Sets default values for this component's properties
UEncounterSytemComponent::UEncounterSytemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEncounterSytemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEncounterSytemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<AICCharacter*> UEncounterSytemComponent::SortPlayersBySpeed(TArray<AICCharacter*> PlayerToSort)
{
	auto SortPred = [](AICCharacter& A, AICCharacter& B)->bool
	{
		return(A.GetCurrentSpeed()) >= (B.GetCurrentSpeed());
	};
	PlayerToSort.Sort(SortPred);

	return PlayerToSort;
}

TArray<ANPC_Character*> UEncounterSytemComponent::SortNPCBySpeed(TArray<ANPC_Character*> NPCToSort)
{
	auto SortPred = [](ANPC_Character& A, ANPC_Character& B)->bool
	{
		return(A.GetCurrentSpeed()) >= (B.GetCurrentSpeed());
	};
	NPCToSort.Sort(SortPred);

	return NPCToSort;
}

void UEncounterSytemComponent::StartEncounter()
{
	AICCharacter* Player = Cast<AICCharacter>(GetOwner());

	// Sort two arrays of actors by speed, one for players team, an another for npcs team
	TArray<AICCharacter*>PlayerToSort = Player->PlayerTeam;
	PlayerTurnOrder = SortPlayersBySpeed(PlayerToSort);

	TArray<ANPC_Character*>NpcToSort = Player->NpcEncounter;
	NpcTurnOrder = SortNPCBySpeed(NpcToSort);

	MessageLogText = FString("Combat Start!");
	Player->MessageLog();

	DecideTurn();
}

void UEncounterSytemComponent::DecideTurn()
{
	if (CurrentNpcRound > 3 || CurrentPlayerRound >3) {	return;	} // PlaceHolder to avoid Endless loop
	// TODO Trigger End Of Combat

	// While on the same Round, choose the faster
	if (PlayerTurnOrder[CurrentPlayerTurn]->GetCurrentSpeed() >= NpcTurnOrder[CurrentNpcTurn]->GetCurrentSpeed() && CurrentPlayerRound == CurrentNpcRound)
	{
		PlayerTurn(PlayerTurnOrder[CurrentPlayerTurn]);
	}
	if (NpcTurnOrder[CurrentNpcTurn]->GetCurrentSpeed() >= PlayerTurnOrder[CurrentPlayerTurn]->GetCurrentSpeed() && CurrentNpcRound == CurrentPlayerRound)
	{
		NpcTurn(NpcTurnOrder[CurrentNpcTurn]);
	}

	// Every npc have played their turn but players are still a round before
	if (CurrentNpcRound > 0 && CurrentPlayerRound == CurrentNpcRound - 1)
	{
		PlayerTurn(PlayerTurnOrder[CurrentPlayerTurn]);
	}
	// Every Player have played their turn but npc are still one round before
	if (CurrentPlayerRound > 0 && CurrentNpcRound == CurrentPlayerRound - 1)
	{
		NpcTurn(NpcTurnOrder[CurrentNpcTurn]);
	}
}

void UEncounterSytemComponent::PlayerTurn(AICCharacter* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("Player ROUND (%i) TURN (%i): %s"), CurrentPlayerRound, CurrentPlayerTurn, *Player->GetName())

	// Update Turns and Rounds
	CurrentPlayerTurn++;

	// Everyone played their turn Reset turn and inc Round
	if (CurrentPlayerTurn >= PlayerTurnOrder.Num())
	{
		CurrentPlayerTurn = 0;
		CurrentPlayerRound++;
	}
	DecideTurn();
}

void UEncounterSytemComponent::NpcTurn(ANPC_Character* Npc)
{
	UE_LOG(LogTemp, Warning, TEXT("NPC ROUND (%i) TURN (%i): %s"), CurrentNpcRound, CurrentNpcTurn, *Npc->GetName())

	// Update Turns and Rounds
	CurrentNpcTurn++;

	// Everyone played their turn Reset turn and inc Round
	if (CurrentNpcTurn >= NpcTurnOrder.Num())
	{
		CurrentNpcTurn = 0;
		CurrentNpcRound++;
	}
	DecideTurn();
}
