// Written by Berkay Tuna, November 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SlotStruct.h"
#include "InventoryComponent.generated.h"

UCLASS(Blueprintable, ClassGroup = (InventorySystem), meta = (BlueprintSpawnableComponent, DisplayName = "Inventory Component"))
class INVENTORYSYSTEM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, noclear, Category = "Inventory")
	UClass* InventoryWindowBPClass;
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (DisplayName = "Toggle Inventory Action Mapping Name"))
	FName ToggleInventoryActionName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, noclear, Category = "Inventory")
	UClass* CharacterSheetBPClass;
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (DisplayName = "Toggle Inventory Action Mapping Name"))
	FName ToggleCharacterSheetActionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FName InventoryName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 NumberOfSlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FSlotStruct> Inventory;

	TWeakObjectPtr<class UInventoryWindow> InventoryWindowBP;
	TWeakObjectPtr<class UCharacterSheet> CharacterSheetBP;

	TWeakObjectPtr<class UCanvasPanel> HUDCanvas;
	EMouseLockMode CharacterSheetMouseLockMode;
	EMouseLockMode InventoryWindowMouseLockMode;
	FVector2D InventoryWindowPosition2D;

	//TWeakObjectPtr<class UInventorySlot> FirstInventorySlot;
	//UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	//TWeakObjectPtr<class UEquipmentSlot> FirstEquipmentSlot;
	//class UEquipmentSlot* FirstEquipmentSlot;

	//void CloseInventoryWindow();
	void CloseWidget(class UUserWidget* InWidget);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleCharacterSheet(class UCanvasPanel* Canvas, EMouseLockMode MouseLockMode);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	class UInventoryWindow* ToggleInventory(UCanvasPanel* Canvas, FVector2D InventoryWindowPosition, EMouseLockMode MouseLockMode);
	bool AddToInventory(FSlotStruct ContentToAdd);

protected:
	virtual void BeginPlay() override;

private:	
	void PrepareInventory();
	void CreateStack(FSlotStruct ContentToAdd);
};