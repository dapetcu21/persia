#include "RewindableAnimInstance.h"

void URewindableAnimInstance::SetRewoundPose(const FPoseSnapshot& Pose)
{
	int32 Index = (RewoundPoseSelector + 1) % RewoundPoseCount;
	RewoundPoseMutexes[Index].Lock();
	RewoundPoses[Index] = Pose;
	RewoundPoseMutexes[Index].Unlock();
	RewoundPoseSelector = Index;
	bRewindingPose = true;
}

void URewindableAnimInstance::ClearRewoundPose()
{
	bRewindingPose = false;
}
