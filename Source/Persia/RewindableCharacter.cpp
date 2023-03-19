#include "RewindableCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "PersiaGameState.h"
#include "RewindSnapshot.h"
#include "RewindableAnimInstance.h"

void ARewindableCharacter::StartRewind()
{
	bRewinding = true;

	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement()) {
		CharacterMovement->StopMovementImmediately();
		CharacterMovement->DisableMovement();
	}
}

void ARewindableCharacter::StopRewind()
{
	bRewinding = false;
	if (URewindableAnimInstance* AnimInstance = Cast<URewindableAnimInstance>(GetMesh()->GetAnimInstance())) {
		AnimInstance->ClearRewoundPose();
	}

	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement()) {
		if (IsDead()) {
			CharacterMovement->DisableMovement();
		} else {
			CharacterMovement->SetDefaultMovementMode();
		}
	}
}

void ARewindableCharacter::SaveRewindSnapshot(struct FRewindActorSnapshot& Snapshot)
{
	Snapshot.Transform = GetActorTransform();
	Snapshot.TimeOfDeath = TimeOfDeath;
	if (URewindableAnimInstance* AnimInstance = Cast<URewindableAnimInstance>(GetMesh()->GetAnimInstance())) {
		AnimInstance->SnapshotPose(Snapshot.Pose);
	}
}

void ARewindableCharacter::RestoreRewindSnapshot(const struct FRewindActorSnapshot& Snapshot)
{
	SetActorTransform(Snapshot.Transform);

	if (Snapshot.TimeOfDeath != TimeOfDeath) {
		TimeOfDeath = Snapshot.TimeOfDeath;
		UpdateTimeOfDeath();
	}

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

	UpdateTimeOfDeath();
}

void ARewindableCharacter::OnRep_TimeOfDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("RepNotify time of death %s: %lf"), *GetFName().ToString(), TimeOfDeath);
	if (bRewinding) return;
	UpdateTimeOfDeath();
}

void ARewindableCharacter::UpdateTimeOfDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Time of death of %s: %lf"), *GetFName().ToString(), TimeOfDeath);
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
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement()) {
		if (bDied) {
			CharacterMovement->DisableMovement();
		} else {
			CharacterMovement->SetDefaultMovementMode();
		}
	}
}
