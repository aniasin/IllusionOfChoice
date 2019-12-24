// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EncounterSytemComponent.generated.h"


UCLASS( ClassGroup=(Custom), Blueprintable)
class IC_API UEncounterSytemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEncounterSytemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool bPlayerHasInitiative;
	bool bNpcHasInitiative;

	UFUNCTION(BlueprintCallable)
		TArray<AICCharacter*> SortPlayersBySpeed(TArray<AICCharacter*> PlayerToSort);
	UFUNCTION(BlueprintCallable)
		TArray<class ANPC_Character*> SortNpcBySpeed(TArray<class ANPC_Character*> NPCToSort);
	UFUNCTION(BlueprintCallable)
		TArray<AICCharacter*> SortPlayersByHealth(TArray<AICCharacter*> PlayerToSort);
	UFUNCTION(BlueprintCallable)
	TArray<ANPC_Character*> SortNpcByHealth(TArray<ANPC_Character*> NpcToSort);
	UFUNCTION(BlueprintCallable)
		TArray<AICCharacter*> SortPlayersByDistance(TArray<AICCharacter*> PlayerToSort);
	UFUNCTION(BlueprintCallable)
		TArray<ANPC_Character*> SortNpcByDistance(TArray<ANPC_Character*> NpcToSort);

	UFUNCTION(BlueprintCallable, Category = "Encounter")
		void IncrementTurnsAndRounds(bool bIsPlayerParty);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
		class ACineCameraActor* EncounterCamera;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
		class ACineCameraActor* CloseupCamera;

	UPROPERTY(BlueprintReadWrite, Category = "Encounter")
		FString MessageLogText;

	void StartEncounter();
	void UpdateMessageLog(FString Message);

	UPROPERTY()
	TArray<class AICCharacter*>PlayerTurnOrder;
	UPROPERTY()
	TArray<class ANPC_Character*>NpcTurnOrder;

private:
	class AICCharacter* PlayerRef;
	class APlayerController* PlayerControllerRef;

	int32 CurrentPlayerRound = 0;
	int32 CurrentNpcRound = 0;
	int32 CurrentPlayerTurn;
	int32 CurrentNpcTurn;
	int32 NumberOfTurns;

	UFUNCTION()
		void DecideTurn();

	// AI
	void AIDecision(AActor* AIActor, bool bIsPartyMember);
	AActor* AIChooseWeakestReachableTarget(AActor* Intigator, bool bIsPartyMember);
	AActor* FindClosestFoe(AActor* Instigator, bool bIsPartyMember);

	// Timers
	class UWorld* World;
	FTimerDelegate BetweenTurnsTimerDelegate;
	FTimerHandle BetweenTurnsTimerHandle;

	FTimerDelegate CameraTimerDelegate;
	FTimerHandle CameraTimerHandle;

	float TimeBetweenTurns = 5.0;

	void PlayerTurn(AICCharacter* PlayerParty);
	void NpcTurn(ANPC_Character* Npc);

	void CameraCloseupView(float Angle, AActor* FocusActor, float Distance, float Height);
	void PositionGeneralViewCamera(float Angle, AActor* FocusActor, float Distance, float Height);

	void PlayerAction(AICCharacter* PlayerParty);
	void PartyMembersAction(AICCharacter* PlayerParty);
	void NpcAction(ANPC_Character* Npc);
	void TimerToNextTurn(float Time);

	UFUNCTION()
	void CameraGeneralView();

	AActor* CurrentCameraFocusActor;
	float CurrentDesiredCameraDistance;
};
