#include "RewindableCharacter.h"

#include "Net/UnrealNetwork.h"
#include "RewindSnapshot.h"

void ARewindableCharacter::SaveRewindSnapshot(struct FRewindActorSnapshot& Snapshot)
{
	Snapshot.Transform = GetActorTransform();
}

void ARewindableCharacter::RestoreRewindSnapshot(const struct FRewindActorSnapshot& Snapshot)
{
	SetActorTransform(Snapshot.Transform);
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
