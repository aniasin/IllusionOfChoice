// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatSystemComponent.generated.h"


UCLASS( ClassGroup=(Custom), Blueprintable)
class IC_API UCombatSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatSystemComponent();

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
};
