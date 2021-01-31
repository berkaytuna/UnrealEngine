// Written by Berkay Tuna, November 2020

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SlotStruct.h"
#include "InventoryInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()

};

class INVENTORYSYSTEM_API IInventoryInterface
{
	GENERATED_BODY()

public:
	uint8 AddToInventory(FSlotStruct ContentToAdd);
	void RemoveFromInventory(int32 Index);

protected:
	TArray<FSlotStruct> Inventory;

	uint8 CreateStack(FSlotStruct ContentToAdd);
};
