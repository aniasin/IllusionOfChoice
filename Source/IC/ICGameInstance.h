// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UI/MenuInterface.h"
#include "ICGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class IC_API UICGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

public:
    UICGameInstance(const FObjectInitializer& ObjectInitializer);

    virtual void Init();

    UFUNCTION(BlueprintCallable, Category = "MenuSystem")
        void LoadMainMenu();
	UFUNCTION(BlueprintCallable, Category = "MenuSystem")
		void LoadGameMenu();
	UFUNCTION(BlueprintCallable, Category = "MenuSystem")
		void LoadEncounterPanel(class AICCharacter* Player);

    UFUNCTION()
        virtual void SaveGame()override;

protected:
	TSubclassOf<class UUserWidget> MainMenuWidget;
	TSubclassOf<class UUserWidget> GameMenuWidget;
	TSubclassOf<class UUserWidget> EncounterPanelWidget;

	class UMainMenu* MainMenu;
	class UGameMenu* GameMenu;
	class UEncounterPanel* EncounterPanel;
};
