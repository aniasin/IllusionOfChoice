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


	UFUNCTION(BlueprintCallable)
		TArray<AICCharacter*> SortPlayersBySpeed(TArray<AICCharacter*> PlayerToSort);
	UFUNCTION(BlueprintCallable)
		TArray<class ANPC_Character*> SortNPCBySpeed(TArray<class ANPC_Character*> NPCToSort);

	UPROPERTY(BlueprintReadWrite, Category = "MessageLog")
		FString MessageLogText;

	void StartEncounter();

	UPROPERTY()
	TArray<class AICCharacter*>PlayerTurnOrder;
	UPROPERTY()
	TArray<class ANPC_Character*>NpcTurnOrder;

private:

	int32 CurrentPlayerRound = 0;
	int32 CurrentNpcRound = 0;
	void DecideTurn();
	int32 CurrentPlayerTurn;
	int32 CurrentNpcTurn;
	int32 NumberOfTurns;

	void PlayerTurn(AICCharacter* Player);
	void NpcTurn(ANPC_Character* Npc);
};
