// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IC/UI/MenuInterface.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class IC_API UMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	IMenuInterface* MenuInterface;

public:
	void SetMenuInterface(IMenuInterface* MenuInterfaceToSet);
	void SetUp();
	void TearDown(bool bToggleInputMode);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void Keyboard(class AICPlayerController* PlayerController);
	UFUNCTION(BlueprintCallable, Category = "Input")
	void Gamepad(class AICPlayerController* PlayerController);
};
	
