#include "PersiaGameState.h"

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
	RewindManager->Tick(DeltaSeconds);
}

void APersiaGameState::RequestStartRewind(class APersiaCharacter* Sender)
{
	if (RewindingPlayer != nullptr) return;
	StartRewind(Sender);
}

void APersiaGameState::RequestStopRewind(class APersiaCharacter* Sender)
{
	if (RewindingPlayer != Sender) return;
	FRewindSnapshot& Snapshot = RewindManager->StopRewindAuthorative();
	StopRewind(Snapshot);
}

void APersiaGameState::StartRewind_Implementation(class APersiaCharacter* Sender)
{
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
