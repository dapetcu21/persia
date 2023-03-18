#pragma once

#include "CoreMinimal.h"

#include "RewindSnapshot.generated.h"

USTRUCT()
struct FRewindActorSnapshot
{
	GENERATED_BODY()

	UPROPERTY()
	int32 NetId = 0;

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FRotator CameraRotation;
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

