// SillikOne.


#include "Menu.h"
#include "IC/UI/MenuInterface.h"
#include "IC/ICPlayerController.h"

void UMenu::SetMenuInterface(IMenuInterface* MenuInterfaceToSet)
{
	this->MenuInterface = MenuInterfaceToSet;
}

void UMenu::SetUp()
{
	this->AddToViewport();

	UWorld* World = GetWorld();
	if (!World) { return; }
	
	AICPlayerController* PlayerController = Cast<AICPlayerController>(World->GetFirstPlayerController());
	if (!PlayerController) { return; }

	if (PlayerController->bIsUsingGamepad)
	{
		Gamepad(PlayerController);
	}
	else
	{
		Keyboard(PlayerController);
	}
}

void UMenu::TearDown(bool bToggleInputMode)
{
	this->RemoveFromViewport();

	UWorld* World = GetWorld();
	if (!World) { return; }
	AICPlayerController* PlayerController = Cast<AICPlayerController>(World->GetFirstPlayerController());
	if (!PlayerController) { return; }

	if (bToggleInputMode || PlayerController->bIsUsingGamepad)
	{
		Gamepad(PlayerController);
	}
	else
	{
		Keyboard(PlayerController);
	}
}

void UMenu::Keyboard(class AICPlayerController* PlayerController)
{
	FInputModeGameAndUI InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = true;
}

void UMenu::Gamepad(class AICPlayerController* PlayerController)
{
	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = false;
}
