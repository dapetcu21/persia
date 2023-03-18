#pragma once

#include "CoreMinimal.h"
#include "RewindableCharacter.h"

#include "PersiaEnemyCharacter.generated.h"

UCLASS(Blueprintable, BlueprintType)
class APersiaEnemyCharacter : public ARewindableCharacter
{
	GENERATED_BODY()

protected:
	static const FName NAME_PatrolDirection;

public:
	void SaveRewindSnapshot(struct FRewindActorSnapshot& Snapshot) override;
	void RestoreRewindSnapshot(const struct FRewindActorSnapshot& Snapshot) override;
};
