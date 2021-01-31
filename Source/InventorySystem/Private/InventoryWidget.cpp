// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryWidget.h"
#include "InventorySystem.h"
#include "Components/Button.h"
#include "SlotStruct.h"

void UInventoryWidget::SlotWidgetOnClicked(USlotWidget* InSlotWidget)
{
	FSlotStruct SlotStruct = InSlotWidget->GetSlotStruct();
	InSlotWidget->Empty();
	InventoryWidgetOnSlotClicked.ExecuteIfBound(this, SlotIndex, SlotStruct);
}

void UInventoryWidget::SlotWidgetOnAddedToFocusPath(USlotWidget* InSlotWidget)
{
	SetWidgetToFocus(InSlotWidget->GetButton());
	InSlotWidget->DisplayShadow();

	FSlotStruct SlotStruct = InSlotWidget->GetSlotStruct();
	OnInventoryWidgetSlotAddedToFocusPath.ExecuteIfBound(SlotStruct);
}

void UInventoryWidget::SlotWidgetOnRemovedFromFocusPath(USlotWidget* InSlotWidget)
{
	InSlotWidget->HideShadow();

	OnInventoryWidgetSlotRemovedFromFocusPath.ExecuteIfBound();
}

void UInventoryWidget::SetFirstWidgetToFocus(UWidget* WidgetToFocus)
{
	FirstWidgetToFocusInternal = WidgetToFocus;
	WidgetToFocusInternal = WidgetToFocus;
}

FReply UInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	InventoryWidgetOnKeyDown.ExecuteIfBound(InKeyEvent);

	return FReply::Handled();
}