// Copyright notice

#include "InventoryInterface.h"
#include "InventorySystem.h"

uint8 IInventoryInterface::AddToInventory(FSlotStruct ContentToAdd)
{
	bool bIsContentStackable = ContentToAdd.Item.IsStackable;

	if (bIsContentStackable)
	{
		return 0;
	}
	else
	{
		return CreateStack(ContentToAdd);
	}
}

uint8 IInventoryInterface::CreateStack(FSlotStruct ContentToAdd)
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		int32 ItemQuantity = Inventory[i].Quantity;

		if (ItemQuantity == 0)
		{
			Inventory[i] = ContentToAdd;
			return i;
		}
	}

	return 0;
}

void IInventoryInterface::RemoveFromInventory(int32 Index)
{
	Inventory.RemoveAt(Index, 1, true);
	Inventory.InsertZeroed(Index);
}