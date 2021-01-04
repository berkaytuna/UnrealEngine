// Copyright notice.

#include "InventoryComponent.h"
#include "Components/CanvasPanel.h"
#include "InventoryWindow.h"
#include "CharacterSheet.h"
#include "InventorySlot.h"
#include "EquipmentSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	PrepareInventory();
}

void UInventoryComponent::CloseWidget(UUserWidget* InWidget)
{
	InWidget->RemoveFromParent();
	UInventoryWindow* InventoryWindow = Cast<UInventoryWindow>(InWidget);
	UCharacterSheet* CharacterSheet = Cast<UCharacterSheet>(InWidget);
	if (InventoryWindow)
	{
		InventoryWindowBP.Reset();
	}
	else if (CharacterSheet)
	{
		CharacterSheetBP.Reset();
	}

	if (!InventoryWindowBP.IsValid() && !CharacterSheetBP.IsValid())
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

		// Set Input Mode Game Only
		if (PlayerController != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("SetInputModeGameOnly! - InventoryWindow closing"));

			FInputModeGameOnly InputMode;
			PlayerController->SetInputMode(InputMode);
		}
	}
}

void UInventoryComponent::ToggleCharacterSheet(UCanvasPanel* Canvas, EMouseLockMode InMouseLockMode)
{
	CharacterSheetMouseLockMode = InMouseLockMode;
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	// Close Widget
	if (CharacterSheetBP.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterSheetBP.IsValid() is true! - ToggleCharacterSheet"));

		CloseWidget(CharacterSheetBP.Get());		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWindowBP.IsValid() is false!"));

		UWorld* World = this->GetWorld();
		CharacterSheetBP = CreateWidget<UCharacterSheet>(World, CharacterSheetBPClass);
		CharacterSheetBP->InventoryComp = this;

		HUDCanvas = Canvas;
		UCanvasPanelSlot* CanvasPanelSlot = Canvas->AddChildToCanvas(CharacterSheetBP.Get());

		// Set Input Mode UI Only
		if (PlayerController != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("SetLockMouseToViewportBehaviour - SetInputModeUIOnly"));


			FInputModeUIOnly InputMode;
			InputMode.SetLockMouseToViewportBehavior(InMouseLockMode);

			if (CharacterSheetBP.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("SetWidgetFocus - SetInputModeUIOnly"));

				UEquipmentSlot* FirstEquipmentSlot = CharacterSheetBP.Get()->FirstEquipmentSlot;

				if (FirstEquipmentSlot)
				{
					UE_LOG(LogTemp, Warning, TEXT("FirstEquipmentSlot does exist!"));

					InputMode.SetWidgetToFocus(FirstEquipmentSlot->TakeWidget());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("FirstEquipmentSlot does not exist!"));
				}
			}
			PlayerController->SetInputMode(InputMode);
		}

	}
}

UInventoryWindow* UInventoryComponent::ToggleInventory(UCanvasPanel* Canvas, FVector2D InventoryWindowPosition, EMouseLockMode InMouseLockMode)
{
	InventoryWindowPosition2D = InventoryWindowPosition;
	InventoryWindowMouseLockMode = InMouseLockMode;
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	//bool bIsInventoryOpen = false;
	UInventorySlot* FirstInventorySlot = nullptr;

	// Close Widget
	if (InventoryWindowBP.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWindowBP.IsValid() is true! - ToggleInventory"))

		CloseWidget(InventoryWindowBP.Get());
		//InventoryWindowBP.Reset();

		//bIsInventoryOpen = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWindowBP.IsValid() is false!"));

		UWorld* World = this->GetWorld();
		InventoryWindowBP = CreateWidget<UInventoryWindow>(World, InventoryWindowBPClass);
		InventoryWindowBP->InventoryComp = this;

		HUDCanvas = Canvas;
		UCanvasPanelSlot* CanvasPanelSlot = Canvas->AddChildToCanvas(InventoryWindowBP.Get());
		CanvasPanelSlot->SetAutoSize(true);
		FVector2D InAllignment = FVector2D{ 0.5, 0.5 };
		CanvasPanelSlot->SetAlignment(InAllignment);
		FAnchors InAnchors = FAnchors{ 0.5, 0.5, 0.5, 0.5 };
		CanvasPanelSlot->SetAnchors(InAnchors);
		CanvasPanelSlot->SetPosition(InventoryWindowPosition);

		// Set Input Mode UI Only
		if (PlayerController != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("SetLockMouseToViewportBehaviour - SetInputModeUIOnly"));

			FInputModeUIOnly InputMode;
			InputMode.SetLockMouseToViewportBehavior(InMouseLockMode);

			if (InventoryWindowBP.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("SetWidgetFocus - SetInputModeUIOnly"));

				FirstInventorySlot = InventoryWindowBP.Get()->FirstInventorySlot.Get();
				UButton* Button = FirstInventorySlot->Button;
				InputMode.SetWidgetToFocus(Button->TakeWidget());
				Button->SetKeyboardFocus();
			}
			PlayerController->SetInputMode(InputMode);;
		}

		/*// Set Input Mode Game And UI
		if (PlayerController != nullptr)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(InMouseLockMode);
			InputMode.SetHideCursorDuringCapture(false);

			if (InventoryWindowBP.IsValid())
			{
				InputMode.SetWidgetToFocus(InventoryWindowBP.Get()->TakeWidget());
			}
			PlayerController->SetInputMode(InputMode);
		}*/

		//bIsInventoryOpen = true;
	}

	return InventoryWindowBP.Get();
}

void UInventoryComponent::PrepareInventory()
{
	Inventory.Empty(NumberOfSlots);
	Inventory.AddZeroed(NumberOfSlots);
}

bool UInventoryComponent::AddToInventory(FSlotStruct ContentToAdd)
{
	bool bIsContentStackable = ContentToAdd.Item.IsStackable;

	if (bIsContentStackable)
	{

	}
	else
	{
		CreateStack(ContentToAdd);
	}

	return true;
}

void UInventoryComponent::CreateStack(FSlotStruct ContentToAdd)
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		int32 ItemQuantity = Inventory[i].Quantity;

		if (ItemQuantity == 0)
		{
			Inventory[i] = ContentToAdd;
			break;
		}
	}
}
