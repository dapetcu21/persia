#include "RewindableCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "PersiaGameState.h"
#include "RewindSnapshot.h"
#include "RewindableAnimInstance.h"

ARewindableCharacter::ARewindableCharacter()
{
}

void ARewindableCharacter::StartRewind()
{
	bRewinding = true;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement()) {
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}
}

void ARewindableCharacter::StopRewind()
{
	bRewinding = false;
	if (URewindableAnimInstance* AnimInstance = Cast<URewindableAnimInstance>(GetMesh()->GetAnimInstance())) {
		AnimInstance->ClearRewoundPose();
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement()) {
		if (IsDead()) {
			Movement->DisableMovement();
		} else {
			Movement->SetDefaultMovementMode();
		}
	}
}

void ARewindableCharacter::SaveRewindSnapshot(struct FRewindActorSnapshot& Snapshot)
{
	Snapshot.Transform = GetActorTransform();
	Snapshot.TimeOfDeath = TimeOfDeath;

	// Snapshotting dead enemies or players results in yet-undiagnosed crashes.
	// For players, the pose snapshot was useless anyway and for enemies it
	// only vaguely matters if a rewind is initiated during the 1s dissolve,
	// so it's safe to disable.
	if (!IsDead()) {
		if (URewindableAnimInstance* AnimInstance = Cast<URewindableAnimInstance>(GetMesh()->GetAnimInstance())) {
			// Crossing fingers I'm allowed to call this on the gameplay thread
			AnimInstance->SnapshotPose(Snapshot.Pose);
		}
	}
}

void ARewindableCharacter::RestoreRewindSnapshot(const struct FRewindActorSnapshot& Snapshot)
{
	SetActorTransform(Snapshot.Transform);

	if (Snapshot.TimeOfDeath != TimeOfDeath) {
		TimeOfDeath = Snapshot.TimeOfDeath;
		OnRep_TimeOfDeath();
	}

	if (URewindableAnimInstance* AnimInstance = Cast<URewindableAnimInstance>(GetMesh()->GetAnimInstance())) {
		AnimInstance->SetRewoundPose(Snapshot.Pose);
	}
}

void ARewindableCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
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
	DOREPLIFETIME(ARewindableCharacter, TimeOfDeath);
}

void ARewindableCharacter::Die()
{
	if (IsDead() || !HasAuthority()) return;

	if (APersiaGameState* GameState = GetWorld()->GetGameState<APersiaGameState>()) {
		TimeOfDeath = GameState->GetRewindGameTime();
	} else {
		TimeOfDeath = 1.0;
	}

	OnRep_TimeOfDeath();
}

void ARewindableCharacter::OnRep_TimeOfDeath()
{
	double OldTimeOfDeath = LastTimeOfDeath;
	LastTimeOfDeath = TimeOfDeath;
	bool bDied = IsDead();
	if (bDied != OldTimeOfDeath >= 0) {
		UpdateDeathState(bDied);
		OnDeathStateChange.Broadcast(bDied);
	}
}

void ARewindableCharacter::UpdateDeathState_Implementation(bool bDied)
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement()) {
		if (bDied) {
			Movement->StopMovementImmediately();
			Movement->DisableMovement();
		} else {
			Movement->SetDefaultMovementMode();
		}
	}
}
