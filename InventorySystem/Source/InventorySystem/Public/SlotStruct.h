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
	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	FItemStruct Item;
	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	int32 Quantity;
};