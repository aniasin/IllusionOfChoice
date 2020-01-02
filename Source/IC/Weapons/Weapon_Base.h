// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon_Base.generated.h"

UCLASS()
class IC_API AWeapon_Base : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon_Base();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stat")
		float Damage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stat")
		float Reach;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stat")
		class UAnimMontage* Animation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
