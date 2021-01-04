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

/*void UInventoryWindow::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("NativeOnFocusLost! - InventoryWindow"));

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	SetUserFocus(PlayerController);	
}*/

void UInventoryWindow::NativeConstruct()
{
	Super::NativeConstruct();

	int32 NumberOfSlots = InventoryComp->NumberOfSlots;

	for (int32 i = 0; i < NumberOfSlots; i++)
	{
		UInventorySlot* InventorySlotBP = CreateWidget<UInventorySlot>(this, InventorySlotBPClass);
		InventorySlotBP->SlotIndex = i;
		InventorySlotBP->InventoryComp = InventoryComp;

		if (i == 0)
		{
			FirstInventorySlot = InventorySlotBP;
		}

		bool bInventorySlotExists = InventoryComp->Inventory.IsValidIndex(i);

		if (bInventorySlotExists)
		{
			InventorySlotBP->SlotStruct = InventoryComp->Inventory[i];
		}

		InventorySlotBP->AddToViewport();

		int32 InRow = i / 8;
		int32 InCollumn = i % 8;

		if (InventoryGrid)
		{
			InventoryGrid->AddChildToGrid(InventorySlotBP, InRow, InCollumn);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryGrid is nullptr!"));
		}

		/*APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

		// Set Input Mode UI Only
		if (PlayerController != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("SetLockMouseToViewportBehaviour - SetInputModeUIOnly"));

			FInputModeUIOnly InputMode;
			InputMode.SetLockMouseToViewportBehavior(InMouseLockMode);

			if (InventoryWindowBP.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("SetWidgetFocus - SetInputModeUIOnly"));

				InputMode.SetWidgetToFocus(InventoryWindowBP.Get()->TakeWidget());
			}
			PlayerController->SetInputMode(InputMode);
		}*/
	}
}

FReply UInventoryWindow::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	UInputSettings* InputSettings = UInputSettings::GetInputSettings();
	TArray<FInputActionKeyMapping> ToggleInventoryKeyMappings;
	FName ToggleInventoryName = InventoryComp.Get()->ToggleInventoryActionName;
	InputSettings->GetActionMappingByName(ToggleInventoryName, ToggleInventoryKeyMappings);

	TArray<FInputActionKeyMapping> ToggleCharacterSheetKeyMappings;
	FName ToggleCharacterSheetName = InventoryComp.Get()->ToggleCharacterSheetActionName;
	InputSettings->GetActionMappingByName(ToggleCharacterSheetName, ToggleCharacterSheetKeyMappings);

	for (int32 i = 0; i < ToggleInventoryKeyMappings.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeOnKeyDown! - for loop"));

		FInputActionKeyMapping ToggleInventoryKeyMapping = ToggleInventoryKeyMappings[i];
		FKey PressedKey = InKeyEvent.GetKey();
		FKey ToggleInventoryKey = ToggleInventoryKeyMapping.Key;

		if (PressedKey == ToggleInventoryKey)
		{
			if (InventoryComp.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("InventoryComp.IsValid() is true - NativeOnKeyDown!"));
				InventoryComp.Get()->CloseWidget(this);
				TWeakObjectPtr<UCharacterSheet> CharacterSheet = InventoryComp.Get()->CharacterSheetBP;
				if (CharacterSheet.IsValid())
				{
					UEquipmentSlot* FirstEquipmentSlot = CharacterSheet.Get()->FirstEquipmentSlot;
					FirstEquipmentSlot->SetKeyboardFocus();
				}

				break;
			}
		}
	}

	for (int32 i = 0; i < ToggleCharacterSheetKeyMappings.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeOnKeyDown! - for loop"));

		FInputActionKeyMapping ToggleCharacterSheetKeyMapping = ToggleCharacterSheetKeyMappings[i];
		FKey PressedKey = InKeyEvent.GetKey();
		FKey ToggleCharacterSheetKey = ToggleCharacterSheetKeyMapping.Key;

		if (PressedKey == ToggleCharacterSheetKey)
		{
			if (InventoryComp.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("InventoryComp.IsValid() is true - NativeOnKeyDown!"));
				TWeakObjectPtr<UCharacterSheet> CharacterSheet = InventoryComp.Get()->CharacterSheetBP;
				if (CharacterSheet.IsValid())
				{
					UE_LOG(LogTemp, Warning, TEXT("CharacterSheet valid"));
					UEquipmentSlot* FirstEquipmentSlot = CharacterSheet.Get()->FirstEquipmentSlot;
					if (FirstEquipmentSlot)
					{
						UE_LOG(LogTemp, Warning, TEXT("FirstEquipmentSlot valid"));
						FirstEquipmentSlot->SetKeyboardFocus();
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("CharacterSheet not valid"));
					UCanvasPanel* HUDCanvas = InventoryComp.Get()->HUDCanvas.Get();
					EMouseLockMode CharacterSheetMouseLockMode = InventoryComp.Get()->CharacterSheetMouseLockMode;
					InventoryComp.Get()->ToggleCharacterSheet(HUDCanvas, CharacterSheetMouseLockMode);
				}

				break;
			}
		}
	}

	return FReply::Handled();
}
