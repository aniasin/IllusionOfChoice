// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterStatComponent.generated.h"


UCLASS( ClassGroup=(Custom), Blueprintable)
class IC_API UCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stat")
		float HealthMax;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stat")
		float HealthCurrent;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stat")
		float SpeedMax;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stat")
		float SpeedCurrent;
};
