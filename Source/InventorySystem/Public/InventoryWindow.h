// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/InputSettings.h"
#include "InventoryWidget.h"
#include "SlotStruct.h"
#include "InventoryWindow.generated.h"

class UGridPanel;

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UInventoryWindow : public UInventoryWidget
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, noclear, Category = "Inventory System")
	UClass* InventorySlotBPClass;

	void AddItemToInventory(int32 SlotIndex, FSlotStruct ItemToAdd);
	void RemoveItemFromInventory(int32 Index);
	void CreateInventorySlots(UClass* InventorySlotClass);
	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	void SetInventoryGrid(UGridPanel* NewInventoryGrid);
	void SetInventory(TArray<FSlotStruct> NewInventory);
	void EmptyInventory();
	void SetNumberOfSlots(int32 InNumberOfSlots);

protected:
	virtual void SlotWidgetOnClicked(USlotWidget* InSlotWidget) final;

private:
	UPROPERTY()
	UGridPanel* InventoryGrid;
	int32 NumberOfSlots;
};
