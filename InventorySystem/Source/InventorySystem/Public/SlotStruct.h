// Written by Berkay Tuna, November 2020

#pragma once

#include "CoreMinimal.h"
#include "ItemStruct.h"
#include "SlotStruct.generated.h"

USTRUCT(BlueprintType)
struct FSlotStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FItemStruct Item;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Quantity;
};