#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "RewindableCharacter.generated.h"

UCLASS(Blueprintable)
class ARewindableCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated)
	int32 NetId = 0;

public:
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SaveRewindSnapshot(struct FRewindActorSnapshot& Snapshot);
	void RestoreRewindSnapshot(const struct FRewindActorSnapshot& Snapshot);
};