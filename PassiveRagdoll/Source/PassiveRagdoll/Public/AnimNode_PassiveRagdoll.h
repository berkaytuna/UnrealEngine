// Written by Berkay Tuna, June 2020

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AnimNodes/AnimNode_BlendListBase.h"
#include "AnimNodes/AnimNode_PoseSnapshot.h"
#include "Animation/PoseSnapshot.h"
#include "PassiveRagdollStruct.h"
#include "AnimNode_PassiveRagdoll.generated.h"

class UAnimInstance;

/** Provide a snapshot pose, either from the internal named pose cache or via a supplied snapshot */
USTRUCT(BlueprintInternalUseOnly)
struct PASSIVERAGDOLL_API FAnimNode_PassiveRagdoll : public FAnimNode_BlendListBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Calculations", meta = (PinShownByDefault))
	FPassiveRagdollStruct Struct;

	UPROPERTY(EditAnywhere, Category = "Calculations")
	FBoneReference BoneToModify;

	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (DisplayName = "Blend Time"))
	float trueBlendTime;

	FAnimNode_PassiveRagdoll();

	/** FAnimNode_Base interface */
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual bool HasPreUpdate() const override { return true; }
	virtual void PreUpdate(const UAnimInstance* InAnimInstance) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;

	/** Snapshot to use. This should be populated at first by calling SnapshotPose */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snapshot", meta = (PinHiddenByDefault))
	FPoseSnapshot Snapshot;

	/** How to access the snapshot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Snapshot", meta = (PinHiddenByDefault))
	ESnapshotSourceMode Mode;

#if WITH_EDITOR
	virtual void AddPose() override
	{
		new (BlendPose) FPoseLink();
	}

	virtual void RemovePose(int32 PoseIndex) override
	{
		BlendPose.RemoveAt(PoseIndex);
	}
#endif

private:
	/** Cache of target space bases to source space bases */
	TArray<int32> SourceBoneMapping;

	/** Cached array of bone names for target's ref skeleton */
	TArray<FName> TargetBoneNames;

	/** Cached skeletal meshes we use to invalidate the bone mapping */
	FName MappedSourceMeshName;
	FName MappedTargetMeshName;

	/** Cached skeletal mesh we used for updating the target bone name array */
	FName TargetBoneNameMesh;

private:
	virtual int32 GetActiveChildIndex() override;

	/** Evaluation helper function - apply a snapshot pose to a pose */
	void ApplyPose(const FPoseSnapshot& PoseSnapshot, FPoseContext& Output);

	/** Evaluation helper function - cache the bone mapping between two skeletal meshes */
	void CacheBoneMapping(FName SourceMeshName, FName TargetMeshName, const TArray<FName>& InSourceBoneNames, const TArray<FName>& InTargetBoneNames);
};
