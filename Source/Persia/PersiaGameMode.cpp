// Copyright Epic Games, Inc. All Rights Reserved.

#include "PersiaGameMode.h"

#include "PersiaCharacter.h"
#include "PersiaGameState.h"
#include "UObject/ConstructorHelpers.h"

APersiaGameMode::APersiaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	GameStateClass = APersiaGameState::StaticClass();
}

void APersiaGameMode::PostLogin(APlayerController* PlayerController)
{
	Super::PostLogin(PlayerController);
	if (APersiaGameState* GameState = GetGameState<APersiaGameState>()) {
		GameState->ReInitRewindManager();
	}
}
