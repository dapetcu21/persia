#include "PersiaGameState.h"

#include "Engine/World.h"
#include "PersiaGameMode.h"
#include "RewindManager.h"

APersiaGameState::APersiaGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APersiaGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	RewindManager = NewObject<URewindManager>(this);
}

void APersiaGameState::BeginPlay()
{
	Super::BeginPlay();
	RewindManager->Setup();
}

// When a new player logs in we re-set the rewind manager,
// so that it picks up the new player's pawn
void APersiaGameState::ReInitRewindManager_Implementation()
{
	RewindManager->Setup();
}

void APersiaGameState::Tick(float DeltaSeconds)
{
	if (Phase == EPersiaGamePhase::Running) {
		RewindManager->Tick(DeltaSeconds);
	}
}

void APersiaGameState::RequestStartRewind(class APersiaPlayerCharacter* Sender)
{
	if (RewindingPlayer != nullptr) return;
	StartRewind(Sender);
}

void APersiaGameState::RequestStopRewind(class APersiaPlayerCharacter* Sender)
{
	if (RewindingPlayer != Sender) return;
	FRewindSnapshot& Snapshot = RewindManager->StopRewindAuthorative();
	StopRewind(Snapshot);
}

void APersiaGameState::StartRewind_Implementation(class APersiaPlayerCharacter* Sender)
{
	if (Phase == EPersiaGamePhase::Lost && HasAuthority()) {
		SetPhase(EPersiaGamePhase::Running);
	}

	RewindingPlayer = Sender;
	RewindManager->StartRewind();
	OnRewindingPlayerChange.Broadcast(RewindingPlayer);
}

void APersiaGameState::StopRewind_Implementation(const struct FRewindSnapshot& Snapshot)
{
	if (!HasAuthority()) {
		RewindManager->StopRewindProxy(Snapshot);
	}
	RewindingPlayer = nullptr;
	OnRewindingPlayerChange.Broadcast(RewindingPlayer);
}

double APersiaGameState::GetRewindGameTime()
{
	return RewindManager == nullptr ? 0.0 : RewindManager->GetGameTime();
}

bool APersiaGameState::IsRewinding()
{
	return RewindManager != nullptr && RewindManager->bRewinding;
}

void APersiaGameState::SetPhase_Implementation(EPersiaGamePhase InPhase)
{
	if (InPhase == Phase) return;
	Phase = InPhase;

	if (APersiaGameMode* GameMode = Cast<APersiaGameMode>(GetWorld()->GetAuthGameMode())) {
		bool bShouldBePaused = Phase != EPersiaGamePhase::Running;
		if (GameMode->IsPaused() != bShouldBePaused) {
			if (bShouldBePaused) {
				GameMode->SetPause(GetWorld()->GetFirstPlayerController());
			} else {
				GameMode->ClearPause();
			}
		}
	}

	OnPhaseChange.Broadcast(Phase);
}
