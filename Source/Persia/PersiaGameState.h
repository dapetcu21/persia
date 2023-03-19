#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "PersiaGameState.generated.h"

UENUM(BlueprintType)
enum class EPersiaGamePhase : uint8
{
	Running,
	Won,
	Lost,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPersiaGameStateRewindingPlayerChange, APersiaPlayerCharacter*, RewindingPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPersiaGameStatePhaseChange, EPersiaGamePhase, Phase);

UCLASS(minimalapi)
class APersiaGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class URewindManager* RewindManager = nullptr;

public:
	UPROPERTY(BlueprintReadOnly)
	EPersiaGamePhase Phase = EPersiaGamePhase::Running;

	UPROPERTY(BlueprintReadOnly)
	class APersiaPlayerCharacter* RewindingPlayer = nullptr;

public:
	UPROPERTY(BlueprintAssignable)
	FPersiaGameStateRewindingPlayerChange OnRewindingPlayerChange;

	UPROPERTY(BlueprintAssignable)
	FPersiaGameStatePhaseChange OnPhaseChange;

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

	UFUNCTION(NetMulticast, Reliable)
	void SetPhase(EPersiaGamePhase InPhase);
	void SetPhase_Implementation(EPersiaGamePhase InPhase);

	UFUNCTION(BlueprintPure)
	double GetRewindGameTime();

	UFUNCTION(BlueprintPure)
	bool IsRewinding();
};
