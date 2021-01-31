// Written by Berkay Tuna, November 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SlotStruct.h"
#include "GameFramework/InputSettings.h"
#include "InventoryInterface.h"
#include "InventoryComponent.generated.h"

class UWidget;
class UInventoryWindow;
class UCharacterSheet;
class UInventoryWidget;
class ULootWindow;
class USlotWindow;
class UTexture2D;
class UContainer;
class UInfoWindow;

UENUM(BlueprintType)
enum EClickReason
{
	Use,
	Equip,
	UnEquip,
	Drop,
	Take,
};

UENUM()
enum ERemoveReason
{
	Placeholder,
	/*Use,
	Equip,
	Drop,
	Destroy*/
};

UENUM()
enum EToggleAction
{
	Inventory,
	CharacterSheet,
	LootWindow,   
};

UENUM()
enum EInputAction
{
	ChangeFocusedWindow,
	ToggleInventory,
};

UCLASS(Blueprintable, ClassGroup = (InventorySystem), meta = (BlueprintSpawnableComponent, DisplayName = "Inventory Component"))
class INVENTORYSYSTEM_API UInventoryComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()

public:
	// We are using BP Classes because on the screen there will be custom BP classes inherited from C++
	UPROPERTY(EditAnywhere, Category = "Inventory")
	UClass* InfoWindowBPClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, noclear, Category = "Inventory")
	UClass* InventoryWindowBPClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, noclear, Category = "Inventory")
	UClass* CharacterSheetBPClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, noclear, Category = "Inventory")
	UClass* LootWindowBPClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, noclear, Category = "Inventory")
	UClass* InventorySlotBPClass;
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (DisplayName = "Toggle Inventory Action Mapping Name"))
	FName ToggleInventoryActionName;
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (DisplayName = "Toggle Character Sheet Action Mapping Name"))
	FName ToggleCharacterSheetActionName;
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (DisplayName = "Toggle Loot Action Mapping Name"))
	FName ToggleLootWindowActionName;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FName ChangeFocusedWindowActionName;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	UTexture2D* BlankSlotTexture;
	// Mouse Lock Mode for all Widgets
	UPROPERTY(EditAnywhere, Category = "Inventory")
	EMouseLockMode WidgetMouseLockMode;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 NumberOfSlots;

	// Inventory Widgets are the main widgets on screen
	UPROPERTY()
	UInventoryWindow* InventoryWindow;
	UPROPERTY()
	UCharacterSheet* CharacterSheet;
	UPROPERTY()
	UInventoryWindow* LootWindow;
	UPROPERTY()
	UInfoWindow* InfoWindow;

	/** Interact function to use with the items and containers */
	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	void Interact();
	/** Gets the reason a slot is clicked */
	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	EClickReason GetClickReason() { return ClickReason; };
	TArray<FInputActionKeyMapping> GetToggleActionKeyMappings(EToggleAction ToggleAction);
	TArray<FInputActionKeyMapping> GetInputActionKeyMappings(EInputAction InputAction);
	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	void ToggleWidget(EToggleAction ToggleAction);
	void ToggleWidgetNative(EToggleAction ToggleAction);
	/** Sets the loot window with the inventory of the container */
	void SetLootWindow(TArray<FSlotStruct> InInventory);
	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	void ToggleInventory();
	void OnInventoryWidgetKeyDown(const FKeyEvent& InKeyEvent);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Inverntory System")
	void OnInventoryWidgetSlotClicked(FSlotStruct SlotStruct);

protected:
	virtual void BeginPlay() override;

private:	
	EClickReason ClickReason;
	AContainer* ActiveContainer;
	uint8 WidgetCount;
	TArray<UInventoryWidget*> InventoryWidgets;
	// Widget array containing to be focused widgets when their owning Inventory Widget is active
	TArray<TWeakObjectPtr<UWidget>> FocusedWidgets;
	TArray<TWeakObjectPtr<UInventoryWidget>> InventoryWidgetsInViewport;

	void BindDelegates(UInventoryWidget* InventoryWidget);
	void Equip(FItemStruct Item);
	void PrepareInventory();
	void OnSlotAddedToFocusPath(FSlotStruct InSlotStruct);
	void OnSlotRemovedFromFocusPath();
	void NativeOnInventoryWidgetSlotClicked(UInventoryWidget* InInventoryWidget, uint8 InSlotIndex, FSlotStruct InSlotStruct);
	UInventoryWidget* CreateInventoryWidget(UClass* WidgetClass);
	void ToggleWidgetInternal(UInventoryWidget* InWidget, EMouseLockMode InMouseLockMode);
	bool CheckIfKeyPressed(TArray<FInputActionKeyMapping> InInputActionKeyMappings, FKey PressedKey);
};