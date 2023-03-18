#include "RewindableCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "RewindSnapshot.h"
#include "RewindableAnimInstance.h"

void ARewindableCharacter::StartRewind()
{
	bRewinding = true;
}

void ARewindableCharacter::StopRewind()
{
	bRewinding = false;
	if (URewindableAnimInstance* AnimInstance = Cast<URewindableAnimInstance>(GetMesh()->GetAnimInstance())) {
		AnimInstance->ClearRewoundPose();
	}
}

void ARewindableCharacter::SaveRewindSnapshot(struct FRewindActorSnapshot& Snapshot)
{
	Snapshot.Transform = GetActorTransform();
	if (URewindableAnimInstance* AnimInstance = Cast<URewindableAnimInstance>(GetMesh()->GetAnimInstance())) {
		AnimInstance->SnapshotPose(Snapshot.Pose);
	}
}

void ARewindableCharacter::RestoreRewindSnapshot(const struct FRewindActorSnapshot& Snapshot)
{
	SetActorTransform(Snapshot.Transform);

	if (URewindableAnimInstance* AnimInstance = Cast<URewindableAnimInstance>(GetMesh()->GetAnimInstance())) {
		AnimInstance->SetRewoundPose(Snapshot.Pose);
	}
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
