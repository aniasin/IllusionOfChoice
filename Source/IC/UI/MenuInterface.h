// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MenuInterface.generated.h"

/**
 * 
 */
UINTERFACE(BlueprintType)
class UMenuInterface : public UInterface
{
	GENERATED_BODY()
	
};

class IC_API IMenuInterface
{
	GENERATED_BODY()


		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
// 	virtual void JoinServer(const FString Address) = 0;
// 
 	virtual void SaveGame() = 0;
// 
// 	virtual void QuitToLobby() = 0;
};
