// SillikOne.


#include "ICPlayerController.h"


AICPlayerController::AICPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	bIsUsingGamepad = false;
	bResetGamepadDetectionAfterNoInput = false;
	GamepadTimeout = 5.f;
}

// bool AICPlayerController::InputAxis(FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
// {
// 	bool ret = Super::InputAxis(Key, Delta, DeltaTime, NumSamples, bGamepad);
// 	_UpdateGamepad(bGamepad);
// 	return ret;
// }

bool AICPlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	bool ret = Super::InputKey(Key, EventType, AmountDepressed, bGamepad);
	_UpdateGamepad(bGamepad);
	return ret;
}

void AICPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	if (IsLocalController() && bResetGamepadDetectionAfterNoInput && bIsUsingGamepad)
	{
		float now = GetWorld()->TimeSeconds;
		if (now > LastGamepadInputTime + GamepadTimeout)
		{
			bIsUsingGamepad = false;
		}
	}
}
