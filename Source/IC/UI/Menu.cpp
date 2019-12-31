// SillikOne.


#include "Menu.h"
#include "IC/UI/MenuInterface.h"

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
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) { return; }
// 	FInputModeGameAndUI InputModeData;
// 	InputModeData.SetWidgetToFocus(this->TakeWidget());
// 	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
// 	PlayerController->SetInputMode(InputModeData);
// 	PlayerController->bShowMouseCursor = false;
}

void UMenu::TearDown(bool bToggleInputMode)
{
	this->RemoveFromViewport();

	UWorld* World = GetWorld();
	if (!World) { return; }
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) { return; }

	if (bToggleInputMode)
	{
// 		FInputModeGameOnly InputModeData;
// 		PlayerController->SetInputMode(InputModeData);
// 		PlayerController->bShowMouseCursor = false;
	}
}
