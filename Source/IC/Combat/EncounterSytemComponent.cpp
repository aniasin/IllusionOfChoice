// SillikOne.


#include "EncounterSytemComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Containers/Array.h"
#include "IC/ICCharacter.h"
#include "GameFramework/PlayerController.h"
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

	World = GetOwner()->GetWorld();
	PlayerRef = Cast<AICCharacter>(GetOwner());
	PlayerControllerRef = Cast<APlayerController>(PlayerRef->GetController());
}


// Called every frame
void UEncounterSytemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// Starting point
void UEncounterSytemComponent::StartEncounter()
{
	PlayerControllerRef->SetInputMode(FInputModeGameAndUI().SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock));
	PlayerControllerRef->bShowMouseCursor = true;
	
	FViewTargetTransitionParams TransitionParams;
	PlayerControllerRef->SetViewTarget(EncounterCamera, TransitionParams);

	PlayerRef->bIsInCombat = true;

	// Sort two arrays of actors by speed, one for players team, an another for npcs team
	TArray<AICCharacter*>PlayerToSort = PlayerRef->PlayerTeam;
	PlayerTurnOrder = SortPlayersBySpeed(PlayerToSort);

	TArray<ANPC_Character*>NpcToSort = PlayerRef->NpcEncounter;
	NpcTurnOrder = SortNPCBySpeed(NpcToSort);

	UpdateMessageLog("Encounter Start!");

	TimerToNextTurn(TimeBetweenTurns);
}

// this filter who's turn it is to play
void UEncounterSytemComponent::DecideTurn()
{
	if (!World) { return; }

	World->GetTimerManager().ClearTimer(BetweenTurnsTimerHandle);

	if (CurrentNpcRound > 3 || CurrentPlayerRound > 3) { return; } // PlaceHolder to avoid Endless loop
	// TODO Trigger End Of Combat

	// TODO Logic accounting Player has initiative or not

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
	
	if (Player->bIsPlayerControlled)
	{
		PlayerAction(Player);
	}
	else
	{
		PartyMembersAction(Player);
	}	
}

void UEncounterSytemComponent::NpcTurn(ANPC_Character* Npc)
{
	UE_LOG(LogTemp, Warning, TEXT("NPC ROUND (%i) TURN (%i): %s"), CurrentNpcRound, CurrentNpcTurn, *Npc->GetName())

	NpcAction(Npc);
}

void UEncounterSytemComponent::IncrementTurnsAndRounds(bool bIsPlayer)
{
	if (bIsPlayer)
	{
		// Update Turns and Rounds
		CurrentPlayerTurn++;

		// Everyone played their turn Reset turn and inc Round
		if (CurrentPlayerTurn >= PlayerTurnOrder.Num())
		{
			CurrentPlayerTurn = 0;
			CurrentPlayerRound++;
		}
	}
	else
	{
		// Update Turns and Rounds
		CurrentNpcTurn++;

		// Everyone played their turn Reset turn and inc Round
		if (CurrentNpcTurn >= NpcTurnOrder.Num())
		{
			CurrentNpcTurn = 0;
			CurrentNpcRound++;
		}
	}
	TimerToNextTurn(TimeBetweenTurns);
}

void UEncounterSytemComponent::PlayerAction(AICCharacter* Player)
{
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController());
	PlayerController->SetViewTarget(Player);

	UpdateMessageLog("Your turn! (For now, jut click 'Attack'...)");
	Player->CreateEncounterPanel();
}

void UEncounterSytemComponent::PartyMembersAction(AICCharacter* Player)
{
	PositionCamera(Player);
	UpdateMessageLog("His turn!");

	// End of turn
	IncrementTurnsAndRounds(true);
}

void UEncounterSytemComponent::NpcAction(ANPC_Character* Npc)
{
	PositionCamera(Npc);
	UpdateMessageLog("His turn!");

	// End of turn
	IncrementTurnsAndRounds(false);
}

void UEncounterSytemComponent::PositionCamera(AActor* FocusActor)
{
	FVector Newlocation;
	// 	Newlocation.X = -575.0;
	// 	Newlocation.Y = 1042;
	// 	Newlocation.Z = 313;
	FVector NpcLocation = FocusActor->GetActorLocation();
	FVector NpcForwardVector = FocusActor->GetActorForwardVector();
	FRotator NpcRotation = FocusActor->GetActorRotation();

	Newlocation = NpcLocation + NpcForwardVector * 200;

	FRotator NewRotation = FRotator(NpcRotation.Roll, NpcRotation.Pitch - 180, NpcRotation.Yaw);

	EncounterCamera->SetActorLocation(Newlocation);
	EncounterCamera->SetActorRotation(NewRotation);

	PlayerControllerRef->SetViewTarget(EncounterCamera);
}

void UEncounterSytemComponent::TimerToNextTurn(float Time)
{
	if (!World) { return; }

	BetweenTurnsTimerDelegate.BindUFunction(this, FName("DecideTurn"));
	World->GetTimerManager().SetTimer(BetweenTurnsTimerHandle, BetweenTurnsTimerDelegate, Time, false);
}

void UEncounterSytemComponent::UpdateMessageLog(FString Message)
{
	MessageLogText = Message;
	PlayerRef->MessageLog();
}

///////////////////////////
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