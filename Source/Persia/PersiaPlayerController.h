#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "PersiaPlayerController.generated.h"

UCLASS(Blueprintable, BlueprintType)
class APersiaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RestartGame();
	void RestartGame_Implementation();
};

