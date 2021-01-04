// Copyright notice.

#include "CharacterSheet.h"
#include "EquipmentSlot.h"
#include "GameFramework/InputSettings.h"
#include "InventoryComponent.h"
#include "InventoryWindow.h"
#include "InventorySlot.h"

FReply UCharacterSheet::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	UInputSettings* InputSettings = UInputSettings::GetInputSettings();
	TArray<FInputActionKeyMapping> ToggleInventoryKeyMappings;
	FName ToggleInventoryName = InventoryComp.Get()->ToggleInventoryActionName;
	InputSettings->GetActionMappingByName(ToggleInventoryName, ToggleInventoryKeyMappings);

	TArray<FInputActionKeyMapping> ToggleCharacterSheetKeyMappings;
	FName ToggleCharacterSheetName = InventoryComp.Get()->ToggleCharacterSheetActionName;
	InputSettings->GetActionMappingByName(ToggleCharacterSheetName, ToggleCharacterSheetKeyMappings);

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
				InventoryComp.Get()->CloseWidget(this);
				TWeakObjectPtr<UInventoryWindow> InventoryWindow = InventoryComp.Get()->InventoryWindowBP;
				if (InventoryWindow.IsValid())
				{
					UInventorySlot* FirstInventorySlot = InventoryWindow.Get()->FirstInventorySlot.Get();
					FirstInventorySlot->SetKeyboardFocus();
				}

				break;
			}
		}
	}

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
				TWeakObjectPtr<UInventoryWindow> InventoryWindow = InventoryComp.Get()->InventoryWindowBP;
				if (InventoryWindow.IsValid())
				{
					UE_LOG(LogTemp, Warning, TEXT("InventoryWindow valid"));
					UInventorySlot* FirstInventorySlot = InventoryWindow.Get()->FirstInventorySlot.Get();
					if (FirstInventorySlot)
					{
						UE_LOG(LogTemp, Warning, TEXT("FirstInventorrySlot valid"));
						FirstInventorySlot->SetKeyboardFocus();
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("InventoryWindow not valid"));
					UCanvasPanel* HUDCanvas = InventoryComp.Get()->HUDCanvas.Get();
					EMouseLockMode InventoryWindowMouseLockMode = InventoryComp.Get()->InventoryWindowMouseLockMode;
					FVector2D InventoryWindowPosition = InventoryComp.Get()->InventoryWindowPosition2D;

					InventoryComp.Get()->ToggleInventory(HUDCanvas, InventoryWindowPosition, InventoryWindowMouseLockMode);
				}

				break;
			}
		}
	}

	return FReply::Handled();
}