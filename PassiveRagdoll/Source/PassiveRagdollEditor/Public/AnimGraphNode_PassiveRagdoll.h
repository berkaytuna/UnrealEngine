// Written by Berkay Tuna, June 2020

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AnimGraphNode_Base.h"
#include "AnimGraphNode_RefPoseBase.h"
#include "AnimNode_PassiveRagdoll.h"
#include "AnimGraphNode_PassiveRagdoll.generated.h"

UCLASS()
class PASSIVERAGDOLLEDITOR_API UAnimGraphNode_PassiveRagdoll : public UAnimGraphNode_Base
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_PassiveRagdoll Node;

	// UEdGraphNode interface
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	// End of UEdGraphNode interface
};
