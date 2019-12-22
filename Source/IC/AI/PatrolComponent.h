// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PatrolComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IC_API UPatrolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	TArray<AActor*>GetPatrolRoute();

private:
	UPROPERTY(EditInstanceOnly, Category = "AI")
		TArray<AActor*>PatrolRoute;		
};
