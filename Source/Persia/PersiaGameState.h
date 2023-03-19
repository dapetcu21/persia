#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "PersiaGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPersiaGameStateRewindingPlayerChange, APersiaPlayerCharacter*, RewindingPlayer);

UCLASS(minimalapi)
class APersiaGameState : public AGameStateBase
{
	GENERATED_BODY()

	UPROPERTY()
	class URewindManager* RewindManager = nullptr;

	UPROPERTY()
	class APersiaPlayerCharacter* RewindingPlayer = nullptr;

public:
	UPROPERTY(BlueprintAssignable)
	FPersiaGameStateRewindingPlayerChange OnRewindingPlayerChange;

public:
	APersiaGameState();

	void PostInitializeComponents() override;
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	void RequestStartRewind(class APersiaPlayerCharacter* Sender);
	void RequestStopRewind(class APersiaPlayerCharacter* Sender);

	UFUNCTION(NetMulticast, Reliable)
	void ReInitRewindManager();
	void ReInitRewindManager_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void StartRewind(class APersiaPlayerCharacter* Sender);
	void StartRewind_Implementation(class APersiaPlayerCharacter* Sender);

	UFUNCTION(NetMulticast, Reliable)
	void StopRewind(const struct FRewindSnapshot& Snapshot);
	void StopRewind_Implementation(const struct FRewindSnapshot& Snapshot);

	UFUNCTION(BlueprintPure)
	double GetRewindGameTime();
};
