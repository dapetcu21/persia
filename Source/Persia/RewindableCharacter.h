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

	UPROPERTY(BlueprintReadOnly)
	bool bRewinding = false;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TimeOfDeath)
	double TimeOfDeath = -1.0;

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

	UFUNCTION(BlueprintNativeEvent)
	void UpdateTimeOfDeath();
	virtual void UpdateTimeOfDeath_Implementation();
};
