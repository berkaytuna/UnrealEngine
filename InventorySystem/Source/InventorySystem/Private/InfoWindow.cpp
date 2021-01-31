// Copyright notice

#include "InfoWindow.h"
#include "InventorySystem.h"

void UInfoWindow::SetSlotStruct(FSlotStruct InSlotStruct)
{
	SlotStruct = InSlotStruct;
}

void UInfoWindow::SetClickReason(EClickReason NewClickReason)
{
	ClickReason = NewClickReason;
}