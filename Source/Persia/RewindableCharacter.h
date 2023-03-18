#pragma once
#include "Animation/PoseSnapshot.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HAL/CriticalSection.h"
#include <atomic>

#include "RewindableCharacter.generated.h"

UCLASS(Blueprintable)
class ARewindableCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated)
	int32 NetId = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bRewinding = false;

protected:
	std::atomic_bool bRewindingPose = false;

	// Triple buffering the pose so we don't have
	// surprises when AnimInstance reads it on another thread
	// I tried to make this lock-free, but out-of-order compilation
	// and execution is a thing and I'd rather not bang my head too
	// much against this, so mutexes that are unlikely to ever block
	// (because of the triple-buffering) is good enough for me.
	static constexpr int32 RewoundPoseCount = 3;
	FCriticalSection RewoundPoseMutexes[RewoundPoseCount];
	FPoseSnapshot RewoundPoses[RewoundPoseCount];
	std::atomic<int32> RewoundPoseSelector = 0;

public:
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void virtual StartRewind();
	void virtual StopRewind();
	void virtual SaveRewindSnapshot(struct FRewindActorSnapshot& Snapshot);
	void virtual RestoreRewindSnapshot(const struct FRewindActorSnapshot& Snapshot);

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	inline bool IsRewindingPose() { return this != nullptr && bRewindingPose; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	inline FPoseSnapshot GetRewoundPose()
	{
		int32 Index = RewoundPoseSelector;
		RewoundPoseMutexes[Index].Lock();
		FPoseSnapshot Pose = RewoundPoses[Index];
		RewoundPoseMutexes[Index].Unlock();
		return Pose;
	}
};
