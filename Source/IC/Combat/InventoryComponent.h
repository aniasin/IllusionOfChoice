// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IC_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
		TArray<AActor*> Items;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
		TArray<AActor*> EquipedItems;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
		AActor* RHand;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
		AActor* LHand;
		
};
