// Written by Berkay Tuna, June 2020

#pragma once

#include "PassiveRagdollStruct.generated.h"

USTRUCT(BlueprintType)
struct FPassiveRagdollStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "Passive Ragdoll")
	bool bInPoseSnapshot;
	UPROPERTY(BlueprintReadWrite, Category = "Passive Ragdoll")
	FTransform RootTransform;
	UPROPERTY(BlueprintReadWrite, Category = "Passive Ragdoll")
	FName PoseSnapshot;
};
