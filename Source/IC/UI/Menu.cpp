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
	
	// Set InputMode UI only and show Mouse Cursor
	AICPlayerController* PlayerController = Cast<AICPlayerController>(World->GetFirstPlayerController());
	if (!PlayerController) { return; }

	if (PlayerController->bIsUsingGamepad)
	{
		FInputModeGameAndUI InputModeData;
		InputModeData.SetWidgetToFocus(this->TakeWidget());
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		PlayerController->SetInputMode(InputModeData);
		PlayerController->bShowMouseCursor = true;
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
		FInputModeGameOnly InputModeData;
		PlayerController->SetInputMode(InputModeData);
		PlayerController->bShowMouseCursor = false;
	}
}
