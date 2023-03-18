// Copyright Epic Games, Inc. All Rights Reserved.

#include "PersiaGameMode.h"

#include "PersiaGameState.h"
#include "PersiaPlayerCharacter.h"
#include "PersiaPlayerController.h"
#include "UObject/ConstructorHelpers.h"

APersiaGameMode::APersiaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_PersiaPlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	GameStateClass = APersiaGameState::StaticClass();
	PlayerControllerClass = APersiaPlayerController::StaticClass();
}

void APersiaGameMode::PostLogin(APlayerController* PlayerController)
{
	Super::PostLogin(PlayerController);
	if (APersiaGameState* GameState = GetGameState<APersiaGameState>()) {
		GameState->ReInitRewindManager();
	}
}
