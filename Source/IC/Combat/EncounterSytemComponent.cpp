// SillikOne.


#include "EncounterSytemComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "CinematicCamera/Public/CineCameraActor.h"
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

}

////////////////////////////////////////////
// Starting point
void UEncounterSytemComponent::StartEncounter()
{
	PlayerControllerRef->SetInputMode(FInputModeGameAndUI().SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock));
	PlayerControllerRef->bShowMouseCursor = true;

	PositionGeneralViewCamera(155, PlayerRef, 900, 450);
	PlayerControllerRef->SetViewTargetWithBlend(EncounterCamera, 0.8);

	PlayerRef->bIsInCombat = true;

	// Sort two arrays of actors by speed, one for players team, an another for npcs team
	PlayerTurnOrder = SortPlayersBySpeed(PlayerRef->PlayerTeam);
	NpcTurnOrder = SortNPCBySpeed(PlayerRef->NpcEncounter);

	UpdateMessageLog("Encounter Start!");

	TimerToNextTurn(TimeBetweenTurns);
}

////////////////////////////////////////
// this filter who's turn it is to play
void UEncounterSytemComponent::DecideTurn()
{
	if (!World) { return; }

	World->GetTimerManager().ClearTimer(BetweenTurnsTimerHandle);

	if (CurrentNpcRound > 3 || CurrentPlayerRound > 3) { return; } // PlaceHolder to avoid Endless loop
	// TODO Trigger End Of Combat

	// TODO Logic accounting Player has initiative or not

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
		// Every npc have played their turn but players have still left
		if (CurrentPlayerRound < CurrentNpcRound)
		{
			PlayerTurn(PlayerTurnOrder[CurrentPlayerTurn]);
		}
		else
		{
			NpcTurn(NpcTurnOrder[CurrentNpcTurn]);
		}
	}
	PositionGeneralViewCamera(155, PlayerRef, 900, 450);
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

	FString Message = FString::Printf(TEXT("%s's turn! (For now, jut click 'Attack'...)"), *PlayerParty->GetName());
	UpdateMessageLog(Message);
	PlayerParty->CreateEncounterPanel();
}

void UEncounterSytemComponent::PartyMembersAction(AICCharacter* PlayerParty)
{
	CameraCloseupView(45, PlayerParty, 400, 25);
	FString Message = FString::Printf(TEXT("%s's turn!"), *PlayerParty->GetName());
	UpdateMessageLog(Message);

	// End of turn
	IncrementTurnsAndRounds(true);
}

void UEncounterSytemComponent::NpcAction(ANPC_Character* Npc)
{
	CameraCloseupView(35, Npc, 400, 25);

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
			NpcTurnOrder = SortNPCBySpeed(PlayerRef->NpcEncounter);
		}
	}
	TimerToNextTurn(TimeBetweenTurns);
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
