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

	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Tactic;

	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Move;

	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Action;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Action01;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Action02;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Action03;

	UPROPERTY(meta = (BindWidget))
		class UVerticalBox* ActionPanel;

	class UVerticalBox* CurrentPanel;

	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Talk;

	UPROPERTY(meta = (BindWidget))
		class UButton* Button_EndTurn;

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
	class AICCharacter* PlayerRef;

};
