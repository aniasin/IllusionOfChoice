// SillikOne.


#include "ICGameInstance.h"
#include "IC/ICCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "IC/UI/MainMenu.h"
#include "IC/UI/GameMenu.h"
#include "IC/UI/EncounterPanel.h"
#include "Runtime/Engine/Classes/Engine/Console.h"

UICGameInstance::UICGameInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWidgetClass(TEXT("/Game/ThirdPersonCPP/UI/MainMenu_BP"));
	if (!ensure(MainMenuWidgetClass.Class)) { return; }
	MainMenuWidget = MainMenuWidgetClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> GameMenuWidgetClass(TEXT("/Game/ThirdPersonCPP/UI/GameMenu_BP"));
	if (!ensure(GameMenuWidgetClass.Class)) { return; }
	GameMenuWidget = GameMenuWidgetClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> EncounterPanelWidgetClass(TEXT("/Game/ThirdPersonCPP/UI/Combat/W_TurnPanel_BP"));
	if (!ensure(EncounterPanelWidgetClass.Class)) { return; }
	EncounterPanelWidget = EncounterPanelWidgetClass.Class;
}

void UICGameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("Found Class: %s"), *MainMenuWidget->GetName())
	UE_LOG(LogTemp, Warning, TEXT("Found Class: %s"), *GameMenuWidget->GetName())
}

void UICGameInstance::LoadMainMenu()
{
	// Create and show Menu
	if (!MainMenuWidget) { return; }
	MainMenu = CreateWidget<UMainMenu>(this, MainMenuWidget);
	MainMenu->SetUp();
	MainMenu->SetMenuInterface(this);
}

void UICGameInstance::LoadGameMenu()
{
	// Create and show Menu
	if (!GameMenuWidget) { return; }
	GameMenu = CreateWidget<UGameMenu>(this, GameMenuWidget);
	GameMenu->SetUp();
	GameMenu->SetMenuInterface(this);
}

UEncounterPanel* UICGameInstance::LoadEncounterPanel(AICCharacter* Player)
{
	// Create and show Panel
	if (!EncounterPanelWidget) { return 0; }
	EncounterPanel = CreateWidget<UEncounterPanel>(this, EncounterPanelWidget);
	EncounterPanel->SetUp();
	EncounterPanel->SetMenuInterface(this);
	EncounterPanel->PlayerRef = Player;

	return EncounterPanel;
}

void UICGameInstance::SaveGame()
{

}
