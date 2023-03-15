// Copyright Epic Games, Inc. All Rights Reserved.

#include "PersiaGameMode.h"

#include "PersiaCharacter.h"
#include "UObject/ConstructorHelpers.h"

APersiaGameMode::APersiaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
