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
		}
	}

	TimestampOffset = -GameState->GetServerWorldTimeSeconds();
	TakeSnapshot(0.0);
}

void URewindManager::StartRewind()
{
	if (bRewinding) return;

	bRewinding = true;
	RewindCursor = TimestampOffset + GameState->GetServerWorldTimeSeconds();
	RewindCursorSnapIndex = Snapshots.Num() - 1;
	RewindVelocity = 1.0f;
	RewindElapsedTime = 0.0f;

	for (const TPair<int32, ARewindableCharacter*>& Pair : TrackedActors) {
		Pair.Value->StartRewind();
	}
}

FRewindSnapshot& URewindManager::StopRewindAuthorative()
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

	StopRewind(RewindCursor);

	check(!Snapshots.IsEmpty());
	return Snapshots.Top();
}

void URewindManager::StopRewindProxy(const FRewindSnapshot& AuthorativeSnapshot)
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

	StopRewind(AuthorativeSnapshot.Timestamp);
}

void URewindManager::StopRewind(double Cursor)
{
	bRewinding = false;
	TimestampOffset = Cursor - GameState->GetServerWorldTimeSeconds();

	for (const TPair<int32, ARewindableCharacter*>& Pair : TrackedActors) {
		Pair.Value->StopRewind();
	}
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

	if (RewindCursorSnapIndex == Snapshots.Num() - 1) {
		RestoreSnapshot(Snapshots[RewindCursorSnapIndex]);
	} else {
		FRewindSnapshot& SnapshotA = Snapshots[RewindCursorSnapIndex];
		FRewindSnapshot& SnapshotB = Snapshots[RewindCursorSnapIndex + 1];
		double SnapshotDelta = SnapshotB.Timestamp - SnapshotA.Timestamp;
		double Alpha = SnapshotDelta == 0.0 ? 0.0 : ((RewindCursor - SnapshotA.Timestamp) / SnapshotDelta);
		// Improvement idea for the future: LERP(SnapshotA, SnapshotB, Alpha)
		RestoreSnapshot(Alpha > 0.5 ? SnapshotB : SnapshotA);
	}
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
