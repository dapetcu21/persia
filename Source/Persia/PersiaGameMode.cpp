// Copyright Epic Games, Inc. All Rights Reserved.

#include "PersiaGameMode.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "PersiaEnemyCharacter.h"
#include "PersiaGameState.h"
#include "PersiaPlayerCharacter.h"
#include "PersiaPlayerController.h"
#include "RewindableCharacter.h"
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

	PrimaryActorTick.bCanEverTick = true;
}

void APersiaGameMode::PostLogin(APlayerController* PlayerController)
{
	Super::PostLogin(PlayerController);
	if (APersiaGameState* GameState = GetGameState<APersiaGameState>()) {
		GameState->ReInitRewindManager();
	}
}

void APersiaGameMode::CheckGameOver()
{
	APersiaGameState* GameState = GetGameState<APersiaGameState>();
	if (GameState == nullptr) return;

	if (GameState->Phase == EPersiaGamePhase::Running && !GameState->IsRewinding()) {
		double RewindTime = GameState->GetRewindGameTime();

		bool bEveryoneDead = true;
		bool bEnemiesDead = true;
		bool bAnyoneLoggedIn = false;

		for (TActorIterator<ARewindableCharacter> It = TActorIterator<ARewindableCharacter>(GetWorld(), ARewindableCharacter::StaticClass()); It; ++It) {
			if (APersiaPlayerCharacter* PlayerPawn = Cast<APersiaPlayerCharacter>(*It)) {
				bAnyoneLoggedIn = true;
				if (!(PlayerPawn->IsDead() && PlayerPawn->TimeOfDeath + PlayerDelayAfterDeath <= RewindTime)) {
					bEveryoneDead = false;
				}
			} else if (APersiaEnemyCharacter* EnemyPawn = Cast<APersiaEnemyCharacter>(*It)) {
				if (!(EnemyPawn->IsDead() && EnemyPawn->TimeOfDeath + EnemyDelayAfterDeath <= RewindTime)) {
					bEnemiesDead = false;
				}
			}
		}

		if (bAnyoneLoggedIn) {
			if (bEveryoneDead) {
				GameState->SetPhase(EPersiaGamePhase::Lost);
			} else if (bEnemiesDead) {
				GameState->SetPhase(EPersiaGamePhase::Won);
			}
		}
	}
}

void APersiaGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Possibly not ideal to do this on tick, but it's the simplest way for now
	CheckGameOver();
}
