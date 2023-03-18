#pragma once

#include "Animation/AnimInstance.h"
#include "Animation/PoseSnapshot.h"
#include "CoreMinimal.h"
#include "HAL/CriticalSection.h"
#include <atomic>

#include "RewindableAnimInstance.generated.h"

UCLASS(Blueprintable, BlueprintType)
class URewindableAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

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

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	inline bool IsRewindingPose() { return bRewindingPose; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	inline FPoseSnapshot GetRewoundPose()
	{
		int32 Index = RewoundPoseSelector;
		RewoundPoseMutexes[Index].Lock();
		FPoseSnapshot Pose = RewoundPoses[Index];
		RewoundPoseMutexes[Index].Unlock();
		return Pose;
	}

public:
	UFUNCTION(BlueprintCallable)
	void SetRewoundPose(const FPoseSnapshot& Pose);

	UFUNCTION(BlueprintCallable)
	void ClearRewoundPose();
};

