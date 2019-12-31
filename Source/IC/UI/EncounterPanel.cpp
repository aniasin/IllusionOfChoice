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
	Buttons.Add(Button_Tactic);

	if (!Button_Move) { return false; }
	Button_Move->OnClicked.AddDynamic(this, &UEncounterPanel::Panel_Move);
	Buttons.Add(Button_Move);

	if (!Button_Action) { return false; }
	Button_Action->OnClicked.AddDynamic(this, &UEncounterPanel::Panel_Action);
	Buttons.Add(Button_Action);
	if (!Button_Action01) { return false; }
	Button_Action01->OnClicked.AddDynamic(this, &UEncounterPanel::Panel_Action01);
	Buttons.Add(Button_Action01);
	if (!Button_Action02) { return false; }
	Button_Action02->OnClicked.AddDynamic(this, &UEncounterPanel::Panel_Action02);
	Buttons.Add(Button_Action02);
	if (!Button_Action03) { return false; }
	Button_Action03->OnClicked.AddDynamic(this, &UEncounterPanel::Panel_Action03);
	Buttons.Add(Button_Action03);

	if (!Button_Talk) { return false; }
	Button_Talk->OnClicked.AddDynamic(this, &UEncounterPanel::Panel_Talk);
	Buttons.Add(Button_Talk);

	if (!Button_EndTurn) { return false; }
	Button_EndTurn->OnClicked.AddDynamic(this, &UEncounterPanel::Panel_EndTurn);
	Buttons.Add(Button_EndTurn);

	return true;
}

void UEncounterPanel::Panel_Tactic()
{

}

void UEncounterPanel::Panel_Move()
{
	ToggleCursorToMove();
	TogglePanel(CurrentPanel);
}

void UEncounterPanel::Panel_Action()
{
	TogglePanel(ActionPanel);
}

void UEncounterPanel::Panel_Action01()
{

}

void UEncounterPanel::Panel_Action02()
{
	ToggleCursorToMove();
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
	TearDown(false);
}

void UEncounterPanel::ClosePanel()
{
	TearDown(false);
}

void UEncounterPanel::TogglePanel(UVerticalBox* Panel)
{
	if (!Panel) { return; }
	if (!CurrentPanel)
	{
		CurrentPanel = Panel;
		CurrentPanel->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CurrentPanel->SetVisibility(ESlateVisibility::Hidden);
		CurrentPanel = NULL;
	}
	if (PlayerRef->bWantToMove) { Panel_Action02(); }
}

void UEncounterPanel::ToggleCursorToMove()
{
	if (!PlayerRef->bWantToMove)
	{
		PlayerRef->bWantToMove = true;
		PlayerRef->Cursor3DDecal->ToggleVisibility();
	}
	else
	{
		PlayerRef->bWantToMove = false;
		PlayerRef->Cursor3DDecal->ToggleVisibility();
	}
}
