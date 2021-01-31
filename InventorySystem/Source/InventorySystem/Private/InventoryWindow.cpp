// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryWindow.h"
#include "InventoryComponent.h"
#include "Components/GridPanel.h"
#include "InventorySlot.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "CharacterSheet.h"
#include "EquipmentSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/Button.h"

/*void UInventoryWindow::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("NativeOnFocusLost! - InventoryWindow"));

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	SetUserFocus(PlayerController);	
}*/

void UInventoryWindow::SetNumberOfSlots(int32 InNumberOfSlots)
{
	NumberOfSlots = InNumberOfSlots;
}

void UInventoryWindow::EmptyInventory()
{
	for (int32 i = 0; i < NumberOfSlots; i++)
	{
		UInventorySlot* InventorySlot = Cast<UInventorySlot>(InventoryGrid->GetChildAt(i));
		InventorySlot->Empty();
	}
}

void UInventoryWindow::SetInventory(TArray<FSlotStruct> InInventory)
{
	for (int32 i = 0; i < InInventory.Num(); i++)
	{
		if (InInventory[i].Quantity > 0)
		{
			UInventorySlot* InventorySlot = Cast<UInventorySlot>(InventoryGrid->GetChildAt(i));
			InventorySlot->SetSlotStruct(InInventory[i]);
		}
	}
}

void UInventoryWindow::SetInventoryGrid(UGridPanel* InInventoryGrid)
{
	InventoryGrid = InInventoryGrid;
}

void UInventoryWindow::SlotWidgetOnClicked(USlotWidget* InSlotWidget)
{
	SlotIndex = InventoryGrid->GetChildIndex(InSlotWidget);
	Super::SlotWidgetOnClicked(InSlotWidget);
}

void UInventoryWindow::AddItemToInventory(int32 InSlotIndex, FSlotStruct InItemToAdd)
{
	UInventorySlot* InventorySlot = Cast<UInventorySlot>(InventoryGrid->GetChildAt(InSlotIndex));
	InventorySlot->SetSlotStruct(InItemToAdd);
}

void UInventoryWindow::RemoveItemFromInventory(int32 Index)
{
	UInventorySlot* InventorySlot = Cast<UInventorySlot>(InventoryGrid->GetChildAt(Index));
	InventorySlot->Empty();
}

void UInventoryWindow::CreateInventorySlots(UClass* InInventorySlotClass)
{
	for (int32 i = 0; i < NumberOfSlots; i++)
	{
		UInventorySlot* InventorySlot = CreateWidget<UInventorySlot>(this, InInventorySlotClass);
		InventorySlot->OnSlotClicked.BindUObject(this, &UInventoryWindow::SlotWidgetOnClicked);
		InventorySlot->OnSlotAddedToFocusPath.BindUObject(this, &UInventoryWindow::SlotWidgetOnAddedToFocusPath);
		InventorySlot->OnSlotRemovedFromFocusPath.BindUObject(this, &UInventoryWindow::SlotWidgetOnRemovedFromFocusPath);

		if (i == 0)
		{
			SetFirstWidgetToFocus(InventorySlot->GetButton());
		}

		/*bool bInventorySlotExists = InventoryComponent->Inventory.IsValidIndex(i);

		if (bInventorySlotExists)
		{
			InventorySlot->SlotStruct = InventoryComponent->Inventory[i];
		}*/

		//InventorySlot->AddToViewport();

		int32 InRow = i / 8;
		int32 InCollumn = i % 8;

		if (InventoryGrid != nullptr)
		{
			InventoryGrid->AddChildToGrid(InventorySlot, InRow, InCollumn);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryGrid is nullptr!"));
		}
	}
}

