// Written by Berkay Tuna, November 2020

#include "InventorySlot.h"
#include "InventorySystem.h"
#include "InventoryComponent.h"
#include "Components/Image.h"

/*void UInventorySlot::OnButtonClicked()
{
	if (SlotStruct.Item.Class != nullptr)
	{
		bool bIsItemConsumable = SlotStruct.Item.IsConsumable;
		if (bIsItemConsumable)
		{
			SlotWindow = InventoryComponent.Get()->GetSlotWindow(ESlotWindow::ConsumableInventorySlot);
		}
		else
		{
			SlotWindow = InventoryComponent.Get()->GetSlotWindow(ESlotWindow::InventorySlot);
		}
	
		if (SlotStruct.Item.Class != nullptr)
		{
			SlotWindow.Get()->AddToViewport();
			SlotWindow.Get()->SetSlotStruct(SlotStruct, this);
			SlotWindow.Get()->GetWidgetToFocus()->SetKeyboardFocus();
			SlotWindow.Reset();
		}
	}
}*/
