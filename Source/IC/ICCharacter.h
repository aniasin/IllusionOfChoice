// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ICCharacter.generated.h"

UCLASS(config=Game)
class AICCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	AICCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
		class UCharacterStatComponent* CharacterStatComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EncounterComponent")
		class UEncounterSytemComponent* EncounterComponent;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InventoryComponent")
		class UInventoryComponent* InventoryComponent;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EncounterComponent")
		class UDecalComponent* CursorToWorldComponent;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UFUNCTION(BlueprintImplementableEvent, Category = "Encounter")
		void MessageLog();
	UFUNCTION(BlueprintCallable, Category = "Encounter")
	void CreateEncounterPanel();

protected:
	void Click();
	void StopClick();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	virtual void BeginPlay()override;

public:
	virtual void Tick(float DeltaTime)override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	bool bIsInCombat = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		bool bWantToMove = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		bool bCanMove = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		int32 NumberOfMove;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PlayerParty")
		TArray<class AICCharacter*>PlayerTeam;
	UPROPERTY()
		TArray<class ANPC_Character*> NpcEncounter;

	float GetCurrentSpeed();
	float GetCurrentHealth();
	float CurrentDistanceToQuerier;

	UPROPERTY(BlueprintReadOnly)
	FVector LocationToMove;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PlayerParty")
		bool bIsPartyMember;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PlayerParty")
	bool bIsPlayerControlled;

private:
	void PositionCursorToWorld();

};
