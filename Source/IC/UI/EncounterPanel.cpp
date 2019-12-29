// SillikOne.


#include "EncounterPanel.h"
#include "IC/ICCharacter.h"
#include "IC/Combat/EncounterSytemComponent.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/DecalComponent.h"

bool UEncounterPanel::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;


	if (!Button_Tactic) { return false; }
	Button_Tactic->OnClicked.AddDynamic(this, &UEncounterPanel::Panel_Tactic);

	if (!Button_Move) { return false; }
	Button_Move->OnClicked.AddDynamic(this, &UEncounterPanel::Panel_Move);

	if (!Button_Action) { return false; }
	Button_Action->OnClicked.AddDynamic(this, &UEncounterPanel::Panel_Action);
	if (!Button_Action01) { return false; }
	Button_Action01->OnClicked.AddDynamic(this, &UEncounterPanel::Panel_Action01);
	if (!Button_Action02) { return false; }
	Button_Action02->OnClicked.AddDynamic(this, &UEncounterPanel::Panel_Action02);
	if (!Button_Action03) { return false; }
	Button_Action03->OnClicked.AddDynamic(this, &UEncounterPanel::Panel_Action03);

	if (!Button_Talk) { return false; }
	Button_Talk->OnClicked.AddDynamic(this, &UEncounterPanel::Panel_Talk);

	if (!Button_EndTurn) { return false; }
	Button_EndTurn->OnClicked.AddDynamic(this, &UEncounterPanel::Panel_EndTurn);

	return true;
}

void UEncounterPanel::Panel_Tactic()
{

}

void UEncounterPanel::Panel_Move()
{
	PlayerRef->bWantToMove = true;
	PlayerRef->CursorToWorldComponent->SetVisibility(true);
	TearDown();
}

void UEncounterPanel::Panel_Action()
{
	if (!ActionPanel) { return; }
	if (!CurrentPanel)
	{
		CurrentPanel = ActionPanel;
		CurrentPanel->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CurrentPanel->SetVisibility(ESlateVisibility::Hidden);
		CurrentPanel = NULL;
	}
}

void UEncounterPanel::Panel_Action01()
{

}

void UEncounterPanel::Panel_Action02()
{

}

void UEncounterPanel::Panel_Action03()
{

}

void UEncounterPanel::Panel_Talk()
{

}

void UEncounterPanel::Panel_EndTurn()
{
	PlayerRef->EncounterComponent->IncrementTurnsAndRounds(true);
	TearDown();
}
