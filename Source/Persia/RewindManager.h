#pragma once

#include "CoreMinimal.h"
#include "Persia/RewindSnapshot.h"

#include "RewindManager.generated.h"

UCLASS(BlueprintType)
class URewindManager : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class AGameStateBase* GameState = nullptr;

	UPROPERTY()
	double TimestampOffset = 0.0;

	UPROPERTY()
	TMap<int32, class ARewindableCharacter*> TrackedActors;

	UPROPERTY()
	TArray<FRewindSnapshot> Snapshots;

public:
	UPROPERTY()
	bool bRewinding = false;

	UPROPERTY()
	double RewindCursor = 0.0;

	UPROPERTY()
	int32 RewindCursorSnapIndex = 0;

	UPROPERTY()
	double RewindVelocity = 1.0;

	UPROPERTY()
	double RewindElapsedTime = 1.0;

protected:
	void TakeSnapshot(double Timestamp);
	void RestoreSnapshot(const FRewindSnapshot& Snapshot);
	void StopRewind(double Cursor);

public:
	void Setup();
	void StartRewind();
	FRewindSnapshot& StopRewindAuthorative();
	void StopRewindProxy(const FRewindSnapshot& AuthorativeSnapshot);
	void Tick(double DeltaTime);
	double GetGameTime();
};

