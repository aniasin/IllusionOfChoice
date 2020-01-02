// SillikOne.


#include "EncounterSytemComponent.h"
#include "IC/Characters/CharacterStatComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "CinematicCamera/Public/CineCameraActor.h"
#include "Containers/Array.h"
#include "IC/ICCharacter.h"
#include "IC/NPC/NPC_AiController.h"
#include "GameFramework/PlayerController.h"
#include "IC/NPC/NPC_Character.h"
#include "Blueprint/UserWidget.h"
#include "IC/UI/LogMessageWidget.h"
#include "IC/Combat/InventoryComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UEncounterSytemComponent::UEncounterSytemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
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

}

////////////////////////////////////////////
// Starting point
void UEncounterSytemComponent::StartEncounter()
{
	//PlayerControllerRef->SetInputMode(FInputModeGameAndUI().SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock));
	//PlayerControllerRef->bShowMouseCursor = true;

	PositionGeneralViewCamera(155, PlayerRef, 900, 450);
	PlayerControllerRef->SetViewTargetWithBlend(EncounterCamera, 0.8);

	PlayerRef->bIsInCombat = true;

	// Sort two arrays of actors by speed, one for players team, an another for npcs team
	PlayerTurnOrder = SortPlayersBySpeed(PlayerRef->PlayerTeam);
	NpcTurnOrder = SortNpcBySpeed(PlayerRef->NpcEncounter);

	UpdateMessageLog("Encounter Start!");

	TimerToNextTurn(TimeBetweenTurns); // Timer before calling DecideTurn v
}

////////////////////////////////////////
// this filter who's turn it is to play
void UEncounterSytemComponent::DecideTurn()
{
	if (!World) { return; }
	World->GetTimerManager().ClearTimer(BetweenTurnsTimerHandle);

	PlayerRef->NumberOfMove = 0;

	if (bPlayerHasInitiative)	
	{ 
		// if player has initiative, npc have no round 0
		CurrentNpcRound = 1;
		bPlayerHasInitiative = false;
	}

	if (CurrentNpcRound > 3 || CurrentPlayerRound > 3) { return; } // PlaceHolder to avoid Endless Encounter has there is no end Encounter yet
	// TODO Trigger End Of Combat

	// While on the same Round, choose the faster
	if (CurrentPlayerRound == CurrentNpcRound)
	{
		if (PlayerTurnOrder[CurrentPlayerTurn]->GetCurrentSpeed() >= NpcTurnOrder[CurrentNpcTurn]->GetCurrentSpeed())
		{
			PlayerTurn(PlayerTurnOrder[CurrentPlayerTurn]);
		}
		else
		{
			NpcTurn(NpcTurnOrder[CurrentNpcTurn]);
		}
	}
	// Not in the same round
	else
	{
		// Every npc have played their turn but players have still some left
		if (CurrentPlayerRound < CurrentNpcRound)
		{
			PlayerTurn(PlayerTurnOrder[CurrentPlayerTurn]);
		}
		else
		{
			NpcTurn(NpcTurnOrder[CurrentNpcTurn]);
		}
	}
	PositionGeneralViewCamera(180, PlayerRef, 900, 450);
	EncounterCamera->LookatTrackingSettings.ActorToTrack = NpcTurnOrder[CurrentNpcTurn];
	EncounterCamera->LookatTrackingSettings.bEnableLookAtTracking = true;

}

void UEncounterSytemComponent::PlayerTurn(AICCharacter* PlayerParty)
{
	UE_LOG(LogTemp, Warning, TEXT("Player ROUND (%i) TURN (%i): %s"), CurrentPlayerRound, CurrentPlayerTurn, *PlayerParty->GetName())
	
	if (PlayerParty->bIsPlayerControlled)
	{
		PlayerAction(PlayerParty);
	}
	else
	{
		PartyMembersAction(PlayerParty);
	}	
}

void UEncounterSytemComponent::NpcTurn(ANPC_Character* Npc)
{
	UE_LOG(LogTemp, Warning, TEXT("NPC ROUND (%i) TURN (%i): %s"), CurrentNpcRound, CurrentNpcTurn, *Npc->GetName())

	NpcAction(Npc);
}

void UEncounterSytemComponent::PlayerAction(AICCharacter* PlayerParty)
{
	APlayerController* PlayerController = Cast<APlayerController>(PlayerParty->GetController());
	PlayerController->SetViewTargetWithBlend(PlayerParty, 0.8);

	FString Message = FString::Printf(TEXT("%s's turn!"), *PlayerParty->GetName());
	UpdateMessageLog(Message);
	PlayerParty->CreateEncounterPanel();
}

void UEncounterSytemComponent::PartyMembersAction(AICCharacter* PlayerParty)
{
	CameraCloseupView(45, PlayerParty, 400, 25);
	AIDecision(PlayerParty, true);

	FString Message = FString::Printf(TEXT("%s's turn!"), *PlayerParty->GetName());
	UpdateMessageLog(Message);

	// End of turn
	IncrementTurnsAndRounds(true);
}

void UEncounterSytemComponent::NpcAction(ANPC_Character* Npc)
{
	CameraCloseupView(35, Npc, 400, 25);
	AIDecision(Npc, false);

	FString Message = FString::Printf(TEXT("%s's turn!"), *Npc->GetName());
	UpdateMessageLog(Message);

	// End of turn
	IncrementTurnsAndRounds(false);
}

//////////////////////////////////
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

void UEncounterSytemComponent::IncrementTurnsAndRounds(bool bIsPlayerParty)
{
	if (bIsPlayerParty)
	{
		// Update Turns and Rounds
		CurrentPlayerTurn++;

		// Everyone played their turn Reset turn and inc Round
		if (CurrentPlayerTurn >= PlayerTurnOrder.Num())
		{
			CurrentPlayerTurn = 0;
			CurrentPlayerRound++;
			// Sort player team again, in case CurrentSpeed was modified(Debuff, wound...)
			PlayerTurnOrder = SortPlayersBySpeed(PlayerRef->PlayerTeam);
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
			// Sort npc again, in case CurrentSpeed was modified(Debuff, wound...)
			NpcTurnOrder = SortNpcBySpeed(PlayerRef->NpcEncounter);
		}
	}
	TimerToNextTurn(TimeBetweenTurns);
}
///////////////////////////
// AI
void UEncounterSytemComponent::AIDecision(AActor* AIActor, bool bIsPartyMember)
{
	AActor* CurrentTarget = NULL;
	// PARTY MEMBER
	if (bIsPartyMember)
	{
		CurrentTarget = AIChooseWeakestReachableTarget(AIActor, true);
		if (CurrentTarget != NULL)
		{
			AICCharacter* Npc = Cast<AICCharacter>(AIActor);
			AAIController* AIController = Cast<AAIController>(Npc->GetController());
			AIController->MoveToActor(CurrentTarget, 50);
		}
		else
			// No weak Target reachable, choose the nearest foe
		{
			AICCharacter* Npc = Cast<AICCharacter>(AIActor);
			AAIController* AIController = Cast<AAIController>(Npc->GetController());
			CurrentTarget = FindClosestFoe(Npc, true);

			FVector LocationToMove;
			LocationToMove = CurrentTarget->GetActorLocation() - Npc->GetActorLocation();
			LocationToMove.Normalize();
			float Distance = Npc->GetCurrentSpeed() * SpeedMultiplier + Npc->GetWeaponReach();
			LocationToMove = LocationToMove * Distance + Npc->GetActorLocation();
			AIController->MoveToLocation(LocationToMove);
		}
	}
	// NPC
	else
	{
		CurrentTarget = AIChooseWeakestReachableTarget(AIActor, false);
		if (CurrentTarget != NULL)
		{
			ANPC_Character* Npc = Cast<ANPC_Character>(AIActor);
			AAIController* AIController = Cast<AAIController>(Npc->GetController());
			AIController->MoveToActor(CurrentTarget, 50);
		}
		else
			// No weak Target reachable, choose the nearest foe
		{
			ANPC_Character* Npc = Cast<ANPC_Character>(AIActor);
			AAIController* AIController = Cast<AAIController>(Npc->GetController());
			CurrentTarget = FindClosestFoe(Npc, false);

			FVector LocationToMove;
			LocationToMove = CurrentTarget->GetActorLocation() - Npc->GetActorLocation();
			LocationToMove.Normalize();
			float Distance = Npc->GetCurrentSpeed() * SpeedMultiplier + Npc->GetWeaponReach();
			LocationToMove = LocationToMove * Distance + Npc->GetActorLocation();
			AIController->MoveToLocation(LocationToMove);
		}
	}
}

AActor* UEncounterSytemComponent::AIChooseWeakestReachableTarget(AActor* Instigator, bool bIsPartyMember)
{
	AActor* CurrentTarget = NULL;
	// PARTY MEMBER
	if (bIsPartyMember)
	{
		TArray<ANPC_Character*>NpcSorted;
		AICCharacter* PlayerParty = Cast<AICCharacter>(Instigator);
		NpcSorted = SortNpcByHealth(NpcTurnOrder);
		for (int32 i = 0 ; i < NpcSorted.Num() ; i++)
		{
			float Distance = FVector::Distance(NpcSorted[i]->GetActorLocation(), PlayerParty->GetActorLocation());
			// Choose the weaker reachable foe
			if (Distance <= PlayerParty->GetCurrentSpeed() * SpeedMultiplier + PlayerParty->GetWeaponReach())
			{
				CurrentTarget = NpcSorted[i];
				break;
			}
		}
	}
	// NPC
	else
	{
		TArray<AICCharacter*>PlayersSorted;
		ANPC_Character* Npc = Cast<ANPC_Character>(Instigator);

		PlayersSorted = SortPlayersByHealth(PlayerTurnOrder);
		for (int32 i = 0; i < PlayersSorted.Num(); i++)
		{
			float Distance = FVector::Distance(PlayersSorted[i]->GetActorLocation(), Npc->GetActorLocation());
			// Choose the weaker reachable foe
			if (Distance <= Npc->GetCurrentSpeed() * SpeedMultiplier + Npc->GetWeaponReach())
			{
				CurrentTarget = PlayersSorted[i];
				break;
			}
		}
	}
	return CurrentTarget;
}

AActor* UEncounterSytemComponent::FindClosestFoe(AActor* Instigator, bool bIsPartyMember)
{
	AActor* CurrentTarget = NULL;
	// PARTY MEMBER
	if (bIsPartyMember)
	{
		AICCharacter* PlayerParty = Cast<AICCharacter>(Instigator);
		TArray<ANPC_Character*>NpcToSort = PlayerRef->NpcEncounter;
		for (int32 i = 0; i < NpcToSort.Num(); i++)
		{
			float Distance = FVector::Distance(NpcToSort[i]->GetActorLocation(), PlayerParty->GetActorLocation());
			NpcToSort[i]->CurrentDistanceToQuerier = Distance;
		}
		TArray<ANPC_Character*>NearNpc = SortNpcByDistance(NpcToSort);
		CurrentTarget = NearNpc[0];
	}
	else
		// NPC
	{
		ANPC_Character* Npc = Cast<ANPC_Character>(Instigator);
		TArray<AICCharacter*>PlayerToSort = PlayerRef->PlayerTeam;
		for (int32 i = 0; i < PlayerToSort.Num(); i++)
		{
			AICCharacter* Player = Cast<AICCharacter>(PlayerToSort[i]);
			float Distance = FVector::Distance(Player->GetActorLocation(), Instigator->GetActorLocation());
			Player->CurrentDistanceToQuerier = Distance;
		}
		TArray<AICCharacter*>NearPlayer = SortPlayersByDistance(PlayerToSort);
		CurrentTarget = NearPlayer[0];
	}
	return CurrentTarget;
}

///////////////////////////
// tools
TArray<AICCharacter*> UEncounterSytemComponent::SortPlayersByDistance(TArray<AICCharacter*> PlayerToSort)
{
	auto SortPred = [](AICCharacter& A, AICCharacter& B)->bool
	{
		return(A.CurrentDistanceToQuerier) <= (B.CurrentDistanceToQuerier);
	};
	PlayerToSort.Sort(SortPred);

	return PlayerToSort;
}

TArray<ANPC_Character*> UEncounterSytemComponent::SortNpcByDistance(TArray<ANPC_Character*> NpcToSort)
{
	auto SortPred = [](ANPC_Character& A, ANPC_Character& B)->bool
	{
		return(A.CurrentDistanceToQuerier) <= (B.CurrentDistanceToQuerier);
	};
	NpcToSort.Sort(SortPred);

	return NpcToSort;
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

TArray<ANPC_Character*> UEncounterSytemComponent::SortNpcBySpeed(TArray<ANPC_Character*> NPCToSort)
{
	auto SortPred = [](ANPC_Character& A, ANPC_Character& B)->bool
	{
		return(A.GetCurrentSpeed()) >= (B.GetCurrentSpeed());
	};
	NPCToSort.Sort(SortPred);

	return NPCToSort;
}

TArray<AICCharacter*> UEncounterSytemComponent::SortPlayersByHealth(TArray<AICCharacter*> PlayerToSort)
{
	auto SortPred = [](AICCharacter& A, AICCharacter& B)->bool
	{
		return(A.GetCurrentHealth()) <= (B.GetCurrentHealth());
	};
	PlayerToSort.Sort(SortPred);

	return PlayerToSort;
}

TArray<ANPC_Character*> UEncounterSytemComponent::SortNpcByHealth(TArray<ANPC_Character*> NPCToSort)
{
	auto SortPred = [](ANPC_Character& A, ANPC_Character& B)->bool
	{
		return(A.GetCurrentHealth()) <= (B.GetCurrentHealth());
	};
	NPCToSort.Sort(SortPred);

	return NPCToSort;
}
//////////////////////////////
// Camera
void UEncounterSytemComponent::CameraCloseupView(float Angle, AActor* FocusActor, float Distance, float Height)
{
	CurrentCameraFocusActor = FocusActor;
	CurrentDesiredCameraDistance = Distance;

	FVector Newlocation;
	FVector NpcLocation = FocusActor->GetActorLocation();
	FVector NpcForwardVector = FocusActor->GetActorForwardVector();

	NpcForwardVector = NpcForwardVector.RotateAngleAxis(Angle, FVector(0, 0, 1));

	Newlocation = NpcLocation + NpcForwardVector * Distance;
	Newlocation = FVector(Newlocation.X, Newlocation.Y, Newlocation.Z + Height);

	CloseupCamera->LookatTrackingSettings.ActorToTrack = FocusActor;
	CloseupCamera->LookatTrackingSettings.bEnableLookAtTracking = true;

	CloseupCamera->SetActorLocation(Newlocation);

	PlayerControllerRef->SetViewTargetWithBlend(CloseupCamera, 0.8);

	CameraTimerDelegate.BindUFunction(this, FName("CameraGeneralView"));
	World->GetTimerManager().SetTimer(CameraTimerHandle, CameraTimerDelegate, 3.0f, false);

}

void UEncounterSytemComponent::PositionGeneralViewCamera(float Angle, AActor* FocusActor, float Distance, float Height)
{
	CurrentCameraFocusActor = FocusActor;
	CurrentDesiredCameraDistance = Distance;

	FVector Newlocation;
	FVector NpcLocation = FocusActor->GetActorLocation();
	FVector NpcForwardVector = FocusActor->GetActorForwardVector();

	NpcForwardVector = NpcForwardVector.RotateAngleAxis(Angle, FVector(0, 0, 1));

	Newlocation = NpcLocation + NpcForwardVector * Distance;
	Newlocation = FVector(Newlocation.X, Newlocation.Y, Newlocation.Z + Height);

	EncounterCamera->LookatTrackingSettings.ActorToTrack = FocusActor;
	EncounterCamera->LookatTrackingSettings.bEnableLookAtTracking = true;

	EncounterCamera->SetActorLocation(Newlocation);
}

void UEncounterSytemComponent::CameraGeneralView()
{
	World->GetTimerManager().ClearTimer(CameraTimerHandle);
	PlayerControllerRef->SetViewTargetWithBlend(EncounterCamera, 0.8);
}
