// Written by Berkay Tuna, June 2020

#include "AnimNode_PassiveRagdoll.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationRuntime.h"
#include "Animation/BlendProfile.h"
#include "Animation/AnimNode_Inertialization.h"

FAnimNode_PassiveRagdoll::FAnimNode_PassiveRagdoll()
	: Mode(ESnapshotSourceMode::NamedSnapshot)
	, MappedSourceMeshName(NAME_None)
	, MappedTargetMeshName(NAME_None)
	, TargetBoneNameMesh(NAME_None)
	, trueBlendTime(0.5f)
{
}

void FAnimNode_PassiveRagdoll::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Initialize_AnyThread)
	FAnimNode_Base::Initialize_AnyThread(Context);

	const int32 NumPoses = (BlendPose.Num() == 1) ? 2 : 0;

	BlendTime.SetNum(2, false);
	BlendTime[0] = trueBlendTime;
	BlendTime[1] = 0.0f;

	BlendWeights.Reset(NumPoses);
	PosesToEvaluate.Reset(NumPoses);
	if (NumPoses > 0)
	{
		// If we have at least 1 pose we initialize to full weight on
		// the first pose
		BlendWeights.AddZeroed(NumPoses);
		BlendWeights[0] = 1.0f;

		PosesToEvaluate.Add(0);

		BlendPose[0].Initialize(Context);
	}

	RemainingBlendTimes.Empty(NumPoses);
	RemainingBlendTimes.AddZeroed(NumPoses);
	Blends.Empty(NumPoses);
	Blends.AddZeroed(NumPoses);

	LastActiveChildIndex = INDEX_NONE;

	for (int32 i = 0; i < Blends.Num(); ++i)
	{
		FAlphaBlend& Blend = Blends[i];

		Blend.SetBlendTime(0.0f);
		Blend.SetBlendOption(BlendType);
		Blend.SetCustomCurve(CustomBlendCurve);
	}
	Blends[0].SetAlpha(1.0f);

	if (BlendProfile)
	{
		// Initialise per-bone data
		PerBoneSampleData.Empty(NumPoses);
		PerBoneSampleData.AddZeroed(NumPoses);

		for (int32 Idx = 0; Idx < NumPoses; ++Idx)
		{
			FBlendSampleData& SampleData = PerBoneSampleData[Idx];
			SampleData.SampleDataIndex = Idx;
			SampleData.PerBoneBlendData.AddZeroed(BlendProfile->GetNumBlendEntries());
		}
	}
}

void FAnimNode_PassiveRagdoll::PreUpdate(const UAnimInstance* InAnimInstance)
{
	// cache the currently used skeletal mesh's bone names
	USkeletalMesh* CurrentSkeletalMesh = nullptr;
	if (InAnimInstance->GetSkelMeshComponent() && InAnimInstance->GetSkelMeshComponent()->IsRegistered())
	{
		CurrentSkeletalMesh = InAnimInstance->GetSkelMeshComponent()->SkeletalMesh;
	}

	if (CurrentSkeletalMesh)
	{
		if (TargetBoneNameMesh != CurrentSkeletalMesh->GetFName())
		{
			// cache bone names for the target mesh
			TargetBoneNames.Reset();
			TargetBoneNames.AddDefaulted(CurrentSkeletalMesh->RefSkeleton.GetNum());

			for (int32 BoneIndex = 0; BoneIndex < CurrentSkeletalMesh->RefSkeleton.GetNum(); ++BoneIndex)
			{
				TargetBoneNames[BoneIndex] = CurrentSkeletalMesh->RefSkeleton.GetBoneName(BoneIndex);
			}
		}

		TargetBoneNameMesh = CurrentSkeletalMesh->GetFName();
	}
	else
	{
		TargetBoneNameMesh = NAME_Name;
	}
}

void FAnimNode_PassiveRagdoll::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(CacheBones_AnyThread)

	const FBoneContainer& RequiredBones = Context.AnimInstanceProxy->GetRequiredBones();
	for (int32 ChildIndex = 0; ChildIndex < BlendPose.Num(); ChildIndex++)
	{
		BlendPose[ChildIndex].CacheBones(Context);
	}

	BoneToModify.Initialize(RequiredBones);
}

void FAnimNode_PassiveRagdoll::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Update_AnyThread)
	GetEvaluateGraphExposedInputs().Execute(Context);

	const int32 NumPoses = (BlendPose.Num() == 1) ? 2 : 0;
	checkSlow(BlendWeights.Num() == NumPoses);

	PosesToEvaluate.Empty(NumPoses);

	if (NumPoses > 0)
	{
		// Handle a change in the active child index; adjusting the target weights
		const int32 ChildIndex = GetActiveChildIndex();

		if (ChildIndex != LastActiveChildIndex)
		{
			bool LastChildIndexIsInvalid = (LastActiveChildIndex == INDEX_NONE);

			const float CurrentWeight = BlendWeights[ChildIndex];
			const float DesiredWeight = 1.0f;
			const float WeightDifference = FMath::Clamp<float>(FMath::Abs<float>(DesiredWeight - CurrentWeight), 0.0f, 1.0f);

			// scale by the weight difference since we want always consistency:
			// - if you're moving from 0 to full weight 1, it will use the normal blend time
			// - if you're moving from 0.5 to full weight 1, it will get there in half the time
			float RemainingBlendTime;
			if (LastChildIndexIsInvalid)
			{
				RemainingBlendTime = 0.0f;
			}
			else if (TransitionType == EBlendListTransitionType::Inertialization)
			{
				FAnimNode_Inertialization* InertializationNode = Context.GetAncestor<FAnimNode_Inertialization>();
				if (InertializationNode)
				{
					InertializationNode->RequestInertialization(BlendTime[ChildIndex]);
				}
				else
				{
					FAnimNode_Inertialization::LogRequestError(Context, BlendPose[ChildIndex]);
				}

				RemainingBlendTime = 0.0f;
			}
			else
			{
				RemainingBlendTime = BlendTime[ChildIndex] * WeightDifference;
			}

			for (int32 i = 0; i < RemainingBlendTimes.Num(); ++i)
			{
				RemainingBlendTimes[i] = RemainingBlendTime;
			}

			// If we have a valid previous child and we're instantly blending - update that pose with zero weight
			if (RemainingBlendTime == 0.0f && !LastChildIndexIsInvalid)
			{
				BlendPose[0].Update(Context.FractionalWeight(0.0f));
			}

			for (int32 i = 0; i < Blends.Num(); ++i)
			{
				FAlphaBlend& Blend = Blends[i];

				Blend.SetBlendTime(RemainingBlendTime);

				if (i == ChildIndex)
				{
					Blend.SetValueRange(BlendWeights[i], 1.0f);
				}
				else
				{
					Blend.SetValueRange(BlendWeights[i], 0.0f);
				}
			}

			// when this flag is true, we'll reinitialize the children
			if (bResetChildOnActivation)
			{
				FAnimationInitializeContext ReinitializeContext(Context.AnimInstanceProxy);

				// reinitialize
				BlendPose[ChildIndex].Initialize(ReinitializeContext);
			}

			LastActiveChildIndex = ChildIndex;
		}

		// Advance the weights
		//@TODO: This means we advance even in a frame where the target weights/times just got modified; is that desirable?
		float SumWeight = 0.0f;
		for (int32 i = 0; i < Blends.Num(); ++i)
		{
			float& BlendWeight = BlendWeights[i];

			FAlphaBlend& Blend = Blends[i];
			Blend.Update(Context.GetDeltaTime());
			BlendWeight = Blend.GetBlendedValue();

			SumWeight += BlendWeight;
		}

		// Renormalize the weights
		if ((SumWeight > ZERO_ANIMWEIGHT_THRESH) && (FMath::Abs<float>(SumWeight - 1.0f) > ZERO_ANIMWEIGHT_THRESH))
		{
			float ReciprocalSum = 1.0f / SumWeight;
			for (int32 i = 0; i < BlendWeights.Num(); ++i)
			{
				BlendWeights[i] *= ReciprocalSum;
			}
		}

		// Update our active children
		for (int32 i = 0; i < BlendPose.Num(); ++i)
		{
			const float BlendWeight = BlendWeights[0];
			if (BlendWeight > ZERO_ANIMWEIGHT_THRESH)
			{
				BlendPose[0].Update(Context.FractionalWeight(BlendWeight));
			}
		}

		PosesToEvaluate.Add(0);
		PosesToEvaluate.Add(1);

		// If we're using a blend profile, extract the scales and build blend sample data
		if (BlendProfile)
		{
			for (int32 i = 0; i < BlendPose.Num(); ++i)
			{
				// Update Per-Bone Info
				const float BlendWeight = BlendWeights[i];
				FBlendSampleData& PoseSampleData = PerBoneSampleData[i];
				PoseSampleData.TotalWeight = BlendWeight;

				for (int32 j = 0; j < PoseSampleData.PerBoneBlendData.Num(); ++j)
				{
					float& BoneBlend = PoseSampleData.PerBoneBlendData[j];
					float WeightScale = BlendProfile->GetEntryBlendScale(j);

					if (ChildIndex != i)
					{
						WeightScale = 1.0f / WeightScale;
					}

					BoneBlend = BlendWeight * WeightScale;
				}
			}

			FBlendSampleData::NormalizeDataWeight(PerBoneSampleData);
		}
	}
}

void FAnimNode_PassiveRagdoll::Evaluate_AnyThread(FPoseContext& Output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Evaluate_AnyThread)
	ANIM_MT_SCOPE_CYCLE_COUNTER(BlendPosesInGraph, !IsInGameThread());

	Output.Pose.ResetToRefPose();

	const int32 NumPoses = PosesToEvaluate.Num();

	int32 BlendPoseNum = (BlendPose.Num() == 1) ? 2 : 0;

	if ((NumPoses > 0) && (BlendPoseNum == BlendWeights.Num()))
	{
		// Scratch arrays for evaluation, stack allocated
		TArray<FCompactPose, TInlineAllocator<8>> FilteredPoses;
		TArray<FBlendedCurve, TInlineAllocator<8>> FilteredCurve;
		FilteredPoses.SetNum(NumPoses, false);
		FilteredCurve.SetNum(NumPoses, false);

		int32 PoseIndex = PosesToEvaluate[0];

		FPoseContext EvaluateContext(Output);

		FPoseLink& CurrentPose = BlendPose[PoseIndex];
		CurrentPose.Evaluate(EvaluateContext);

		FilteredPoses[0].MoveBonesFrom(EvaluateContext.Pose);
		FilteredCurve[0].MoveFrom(EvaluateContext.Curve);

		if (const FPoseSnapshot* PoseSnapshotData = Output.AnimInstanceProxy->GetPoseSnapshot(Struct.PoseSnapshot))
		{
			ApplyPose(*PoseSnapshotData, Output);
		}

		FilteredPoses[1] = Output.Pose;
		FilteredCurve[1] = Output.Curve;

		// Use the calculated blend sample data if we're blending per-bone
		if (BlendProfile)
		{
			FAnimationRuntime::BlendPosesTogetherPerBone(FilteredPoses, FilteredCurve, BlendProfile, PerBoneSampleData, PosesToEvaluate, Output.Pose, Output.Curve);
		}
		else
		{
			FAnimationRuntime::BlendPosesTogether(FilteredPoses, FilteredCurve, BlendWeights, PosesToEvaluate, Output.Pose, Output.Curve);
		}
	}
	else
	{
		Output.ResetToRefPose();
	}
}

int32 FAnimNode_PassiveRagdoll::GetActiveChildIndex()
{
	return Struct.bInPoseSnapshot ? 1 : 0;
}

void FAnimNode_PassiveRagdoll::ApplyPose(const FPoseSnapshot& PoseSnapshot, FPoseContext& Output)
{
	const TArray<FTransform>& LocalTMs = PoseSnapshot.LocalTransforms;
	const FBoneContainer& RequiredBones = Output.Pose.GetBoneContainer();

	if (TargetBoneNameMesh == PoseSnapshot.SkeletalMeshName)
	{
		for (FCompactPoseBoneIndex PoseBoneIndex : Output.Pose.ForEachBoneIndex())
		{
			const FMeshPoseBoneIndex MeshBoneIndex = RequiredBones.MakeMeshPoseIndex(PoseBoneIndex);
			const int32 Index = MeshBoneIndex.GetInt();

			if (LocalTMs.IsValidIndex(Index))
			{
				Output.Pose[PoseBoneIndex] = LocalTMs[Index];

				FCSPose<FCompactPose> CSPose;
				CSPose.InitPose(Output.Pose);

				const FBoneContainer& BoneContainer = Output.Pose.GetBoneContainer();
				FCompactPoseBoneIndex CompactPoseBoneToModify = BoneToModify.GetCompactPoseIndex(BoneContainer);
				FTransform NewBoneTM = CSPose.GetComponentSpaceTransform(CompactPoseBoneToModify);
				FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();
				FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, CSPose, NewBoneTM, CompactPoseBoneToModify, BCS_WorldSpace);
				const FQuat BoneQuat = Struct.RootTransform.GetRotation();
				const FVector Location = Struct.RootTransform.GetLocation();
				NewBoneTM.SetRotation(BoneQuat);
				NewBoneTM.SetTranslation(Location);
				FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, CSPose, NewBoneTM, CompactPoseBoneToModify, BCS_WorldSpace);
				CSPose.SetComponentSpaceTransform(CompactPoseBoneToModify, NewBoneTM);

				FCSPose<FCompactPose>::ConvertComponentPosesToLocalPoses(MoveTemp(CSPose), Output.Pose);
			}
		}
	}
	else
	{
		// per-bone matching required
		CacheBoneMapping(PoseSnapshot.SkeletalMeshName, TargetBoneNameMesh, PoseSnapshot.BoneNames, TargetBoneNames);

		for (FCompactPoseBoneIndex PoseBoneIndex : Output.Pose.ForEachBoneIndex())
		{
			const FMeshPoseBoneIndex MeshBoneIndex = RequiredBones.MakeMeshPoseIndex(PoseBoneIndex);
			const int32 Index = MeshBoneIndex.GetInt();

			if (SourceBoneMapping.IsValidIndex(Index))
			{
				const int32 SourceBoneIndex = SourceBoneMapping[Index];
				if (LocalTMs.IsValidIndex(SourceBoneIndex))
				{
					Output.Pose[PoseBoneIndex] = LocalTMs[SourceBoneIndex];

					FCSPose<FCompactPose> CSPose;
					CSPose.InitPose(Output.Pose);

					const FBoneContainer& BoneContainer = Output.Pose.GetBoneContainer();
					FCompactPoseBoneIndex CompactPoseBoneToModify = BoneToModify.GetCompactPoseIndex(BoneContainer);
					FTransform NewBoneTM = CSPose.GetComponentSpaceTransform(CompactPoseBoneToModify);
					FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();
					FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, CSPose, NewBoneTM, CompactPoseBoneToModify, BCS_WorldSpace);
					const FQuat BoneQuat = Struct.RootTransform.GetRotation();
					const FVector Location = Struct.RootTransform.GetLocation();
					NewBoneTM.SetRotation(BoneQuat);
					NewBoneTM.SetTranslation(Location);
					FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, CSPose, NewBoneTM, CompactPoseBoneToModify, BCS_WorldSpace);
					CSPose.SetComponentSpaceTransform(CompactPoseBoneToModify, NewBoneTM);

					FCSPose<FCompactPose>::ConvertComponentPosesToLocalPoses(MoveTemp(CSPose), Output.Pose);
				}
			}
		}
	}
}

void FAnimNode_PassiveRagdoll::GatherDebugData(FNodeDebugData& DebugData)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(GatherDebugData)
	FString DebugLine = DebugData.GetNodeName(this) + " Snapshot Name:" + Struct.PoseSnapshot.ToString();
	DebugData.AddDebugItem(DebugLine, true);
}

void FAnimNode_PassiveRagdoll::CacheBoneMapping(FName InSourceMeshName, FName InTargetMeshName, const TArray<FName>& InSourceBoneNames, const TArray<FName>& InTargetBoneNames)
{
	if (InSourceMeshName != MappedSourceMeshName || InTargetMeshName != MappedTargetMeshName)
	{
		SourceBoneMapping.Reset();

		for (int32 BoneIndex = 0; BoneIndex < InTargetBoneNames.Num(); ++BoneIndex)
		{
			SourceBoneMapping.Add(InSourceBoneNames.IndexOfByKey(InTargetBoneNames[BoneIndex]));
		}

		MappedSourceMeshName = InSourceMeshName;
		MappedTargetMeshName = InTargetMeshName;
	}
}