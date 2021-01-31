// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/InputSettings.h"
#include "InventoryComponent.h"
#include "SlotWidget.h"
#include "InventoryWidget.generated.h"

DECLARE_DELEGATE_OneParam(FInventoryWidgetOnKeyDown, const FKeyEvent&);
DECLARE_DELEGATE_ThreeParams(FInventoryWidgetOnSlotClicked, UInventoryWidget*, uint8, FSlotStruct);
DECLARE_DELEGATE_OneParam(FOnInventoryWidgetSlotAddedToFocusPath, FSlotStruct);
DECLARE_DELEGATE(FOnInventoryWidgetSlotRemovedFromFocusPath);

class UInventoryComponent;
class FOnSlotWidgetButtonClicked;

/** Widget class for the main Inventory System Widgets */
UCLASS()
class INVENTORYSYSTEM_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FInventoryWidgetOnKeyDown InventoryWidgetOnKeyDown;
	FInventoryWidgetOnSlotClicked InventoryWidgetOnSlotClicked;
	FOnInventoryWidgetSlotAddedToFocusPath OnInventoryWidgetSlotAddedToFocusPath;
	FOnInventoryWidgetSlotRemovedFromFocusPath OnInventoryWidgetSlotRemovedFromFocusPath;

	UWidget* GetWidgetToFocus() { return WidgetToFocusInternal.Get(); };
	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	void SetWidgetToFocus(UWidget* WidgetToFocus) { WidgetToFocusInternal = WidgetToFocus; };
	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	void SetFirstWidgetToFocus(UWidget* WidgetToFocus);
	UWidget* GetFirstWidgetToFocus() { return FirstWidgetToFocusInternal.Get(); };

protected:
	TWeakObjectPtr<UInventoryComponent> InventoryComponent;
	TWeakObjectPtr<UWidget> WidgetToFocusInternal;
	TWeakObjectPtr<UWidget> FirstWidgetToFocusInternal;
	uint8 Status;
	uint8 SlotIndex;

	virtual void SlotWidgetOnClicked(USlotWidget* InSlotWidget);
	virtual void SlotWidgetOnAddedToFocusPath(USlotWidget* InSlotWidget);
	virtual void SlotWidgetOnRemovedFromFocusPath(USlotWidget* InSlotWidget);
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};