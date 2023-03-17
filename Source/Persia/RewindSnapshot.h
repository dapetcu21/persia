#pragma once

#include "CoreMinimal.h"

#include "RewindSnapshot.generated.h"

USTRUCT()
struct FRewindActorSnapshot
{
	GENERATED_BODY()

	UPROPERTY()
	int32 NetId;

	UPROPERTY()
	FTransform Transform;
};

USTRUCT()
struct FRewindSnapshot
{
	GENERATED_BODY()

	UPROPERTY()
	double Timestamp = 0.0f;

	UPROPERTY()
	TArray<FRewindActorSnapshot> Actors;
};

