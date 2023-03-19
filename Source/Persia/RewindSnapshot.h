#pragma once

#include "Animation/PoseSnapshot.h"
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

	UPROPERTY()
	FVector PatrolDirection;

	UPROPERTY()
	double TimeOfDeath = -1.0;

	// Too big to send over the network
	UPROPERTY(NotReplicated)
	FPoseSnapshot Pose;
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

