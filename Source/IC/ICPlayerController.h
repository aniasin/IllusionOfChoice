// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ICPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class IC_API AICPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AICPlayerController();

	// Gamepad Detection
	/** Can be called to check and see if the player is currently using a gamepad */
	UPROPERTY(Transient, BlueprintReadOnly)
		bool bIsUsingGamepad;

	/** Specifies whether we set `bIsUsingGamepad` to `false` if we receive no input for a period of time. If set to 'true', GamepadTimeout will control how long
		we need to go without receiving input before we set `bIsUsingGamepad` to `false`. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bResetGamepadDetectionAfterNoInput;

	/** How long we can go without receiving a gamepad input before we assume they've stopped using the gamepad */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bResetGamepadDetectionAfterNoInput"))
		float GamepadTimeout;


	// Overrides
	virtual bool InputAxis(FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad) override;
	virtual bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad) override;
	virtual void PlayerTick(float DeltaTime) override;

protected:

	// Gamepad Detection
/** Common logic needed in both `InputAxis()` and `InputKey()` */
	FORCEINLINE void _UpdateGamepad(bool bGamepad)
	{
		bIsUsingGamepad = bGamepad;
		if (bGamepad)
		{
			LastGamepadInputTime = GetWorld()->TimeSeconds;
		}
	}

	// Used to keep track of when we last saw gamepad input
	UPROPERTY(Transient, BlueprintReadOnly)
		float LastGamepadInputTime;
	
};
