// Written by Berkay Tuna, June 2020

#include "AnimGraphNode_PassiveRagdoll.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

UAnimGraphNode_PassiveRagdoll::UAnimGraphNode_PassiveRagdoll(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Node.AddPose();
}

FText UAnimGraphNode_PassiveRagdoll::GetTooltipText() const
{
	return LOCTEXT("UAnimGraphNode_PassiveRagdoll_Tooltip", "Passive Ragdoll");
}

FText UAnimGraphNode_PassiveRagdoll::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("UAnimGraphNode_PassiveRagdoll_Title", "Passive Ragdoll");
}

#undef LOCTEXT_NAMESPACE