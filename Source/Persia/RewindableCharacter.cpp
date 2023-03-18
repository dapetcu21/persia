#include "RewindableCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "RewindSnapshot.h"

void ARewindableCharacter::StartRewind()
{
	bRewinding = true;
}

void ARewindableCharacter::StopRewind()
{
	bRewinding = false;
	bRewindingPose = false;
}

void ARewindableCharacter::SaveRewindSnapshot(struct FRewindActorSnapshot& Snapshot)
{
	Snapshot.Transform = GetActorTransform();
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance()) {
		AnimInstance->SnapshotPose(Snapshot.Pose);
	}
}

void ARewindableCharacter::RestoreRewindSnapshot(const struct FRewindActorSnapshot& Snapshot)
{
	SetActorTransform(Snapshot.Transform);

	int32 Index = (RewoundPoseSelector + 1) % RewoundPoseCount;
	RewoundPoseMutexes[Index].Lock();
	RewoundPoses[Index] = Snapshot.Pose;
	RewoundPoseMutexes[Index].Unlock();
	RewoundPoseSelector = Index;
	bRewindingPose = true;
}

void ARewindableCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) {
		static int32 NetIdCounter = 1;
		NetId = NetIdCounter;
		NetIdCounter += 1;
	}
}

void ARewindableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ARewindableCharacter, NetId, COND_InitialOnly);
}
