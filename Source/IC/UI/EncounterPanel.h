// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "IC/UI/Menu.h"
#include "EncounterPanel.generated.h"

/**
 * 
 */
UCLASS()
class IC_API UEncounterPanel : public UMenu
{
	GENERATED_BODY()

		virtual bool Initialize()override;

	UFUNCTION()
		void Panel_Tactic();

	UFUNCTION()
		void Panel_Move();

	UFUNCTION()
		void Panel_Action();
	UFUNCTION()
		void Panel_Action01();
	UFUNCTION()
		void Panel_Action02();
	UFUNCTION()
		void Panel_Action03();

	UFUNCTION()
		void Panel_Talk();

	UFUNCTION()
		void Panel_EndTurn();

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* Button_Tactic;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* Button_Move;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* Button_Action;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* Button_Action01;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* Button_Action02;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* Button_Action03;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UVerticalBox* ActionPanel;

	class UVerticalBox* CurrentPanel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* Button_Talk;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* Button_EndTurn;

	class AICCharacter* PlayerRef;
	void ClosePanel();

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<UButton*> Buttons;

private:
	void TogglePanel(UVerticalBox* Panel);
	void ToggleCursorToMove();
};
