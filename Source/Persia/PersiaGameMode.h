// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "PersiaGameMode.generated.h"

UCLASS(minimalapi)
class APersiaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APersiaGameMode();

protected:
	UPROPERTY(EditDefaultsOnly)
	double PlayerDelayAfterDeath = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	double EnemyDelayAfterDeath = 2.0f;

protected:
	void PostLogin(APlayerController* PlayerController) override;
	void Tick(float DeltaSeconds) override;
	void CheckGameOver();
};

