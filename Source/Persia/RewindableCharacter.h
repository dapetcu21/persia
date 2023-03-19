#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "RewindableCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathStateDelegate, bool, bDied);

UCLASS(Blueprintable)
class ARewindableCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARewindableCharacter();

	UPROPERTY(Replicated)
	int32 NetId = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bRewinding = false;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TimeOfDeath)
	double TimeOfDeath = -1.0;

	UPROPERTY(BlueprintReadOnly)
	double LastTimeOfDeath = -1.0;

	UPROPERTY(BlueprintAssignable)
	FDeathStateDelegate OnDeathStateChange;

public:
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void StartRewind();
	virtual void StopRewind();
	virtual void SaveRewindSnapshot(struct FRewindActorSnapshot& Snapshot);
	virtual void RestoreRewindSnapshot(const struct FRewindActorSnapshot& Snapshot);

	UFUNCTION(BlueprintPure)
	inline bool IsDead() { return TimeOfDeath >= 0; }

	UFUNCTION(BlueprintCallable)
	void Die();

protected:
	UFUNCTION()
	void OnRep_TimeOfDeath();
	void UpdateTimeOfDeath();

	UFUNCTION(BlueprintNativeEvent)
	void UpdateDeathState(bool bDied);
	virtual void UpdateDeathState_Implementation(bool bDied);
};
