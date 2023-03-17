#include "RewindManager.h"

#include "EngineUtils.h"
#include "GameFramework/GameStateBase.h"
#include "RewindableCharacter.h"

void URewindManager::Setup()
{
	GameState = Cast<AGameStateBase>(GetOuter());
	TrackedActors.Empty();
	Snapshots.Empty();

	for (FActorIterator It(GameState->GetWorld()); It; ++It) {
		if (ARewindableCharacter* Actor = Cast<ARewindableCharacter>(*It)) {
			TrackedActors.Add(Actor->NetId, Actor);
			UE_LOG(LogTemp, Display, TEXT("Tracked actor: %d"), Actor->NetId);
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Tracked actor count: %d"), TrackedActors.Num());

	TimestampOffset = -GameState->GetServerWorldTimeSeconds();
	TakeSnapshot(0.0);
}

void URewindManager::StartRewind()
{
	if (bRewinding) return;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Start rewind"));

	bRewinding = true;
	RewindCursor = TimestampOffset + GameState->GetServerWorldTimeSeconds();
	RewindCursorSnapIndex = Snapshots.Num() - 1;
	RewindVelocity = 1.0f;
	RewindElapsedTime = 0.0f;
}

FRewindSnapshot& URewindManager::EndRewindAuthorative()
{
	check(bRewinding);

	// Pop the rewound time range from the timeline
	int32 Index = Snapshots.Num() - 1;
	for (; Index > 0; Index -= 1) {
		if (Snapshots[Index].Timestamp <= RewindCursor) {
			break;
		}
	}
	Snapshots.SetNum(Index + 1);

	EndRewind(RewindCursor);

	check(!Snapshots.IsEmpty());
	return Snapshots.Top();
}

void URewindManager::EndRewindProxy(const FRewindSnapshot& AuthorativeSnapshot)
{
	check(bRewinding);

	// Pop the rewound time range from the timeline
	int32 Index = Snapshots.Num() - 1;
	for (; Index >= 0; Index -= 1) {
		if (Snapshots[Index].Timestamp < AuthorativeSnapshot.Timestamp) {
			break;
		}
	}
	Snapshots.SetNum(Index + 1);

	Snapshots.Add(AuthorativeSnapshot);
	RestoreSnapshot(AuthorativeSnapshot);

	EndRewind(AuthorativeSnapshot.Timestamp);
}

void URewindManager::EndRewind(double Cursor)
{
	bRewinding = false;
	TimestampOffset = Cursor - GameState->GetServerWorldTimeSeconds();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Stop rewind"));
}

void URewindManager::Tick(double DeltaTime)
{
	if (!bRewinding) {
		TakeSnapshot(TimestampOffset + GameState->GetServerWorldTimeSeconds());
		return;
	}

	RewindCursor = FMath::Max(0.0, RewindCursor - RewindVelocity * DeltaTime);
	RewindElapsedTime += DeltaTime;
	if (RewindElapsedTime >= 2.0) {
		RewindVelocity += RewindVelocity * DeltaTime * 0.5;
	}

	if (!ensureAlways(RewindCursorSnapIndex >= 0)) return;
	while (RewindCursorSnapIndex > 0 && Snapshots[RewindCursorSnapIndex].Timestamp > RewindCursor) {
		RewindCursorSnapIndex -= 1;
	}

	// Improvement idea for the future: LERP Snapshots[RewindCursorSnapIndex] and Snapshots[RewindCursorSnapIndex + 1]
	RestoreSnapshot(Snapshots[RewindCursorSnapIndex]);
}

double URewindManager::GetGameTime()
{
	if (bRewinding) {
		return RewindCursor;
	}
	return GameState->GetServerWorldTimeSeconds() - TimestampOffset;
}

void URewindManager::TakeSnapshot(double Timestamp)
{
	FRewindSnapshot& Snapshot = Snapshots.Emplace_GetRef();
	Snapshot.Timestamp = Timestamp;

	for (const TPair<int32, ARewindableCharacter*>& Pair : TrackedActors) {
		FRewindActorSnapshot& ActorSnapshot = Snapshot.Actors.Emplace_GetRef();
		ActorSnapshot.NetId = Pair.Key;
		Pair.Value->SaveRewindSnapshot(ActorSnapshot);
	}
}

void URewindManager::RestoreSnapshot(const FRewindSnapshot& Snapshot)
{
	for (const FRewindActorSnapshot& ActorSnapshot : Snapshot.Actors) {
		if (ARewindableCharacter** Actor = TrackedActors.Find(ActorSnapshot.NetId)) {
			(*Actor)->RestoreRewindSnapshot(ActorSnapshot);
		}
	}
}
