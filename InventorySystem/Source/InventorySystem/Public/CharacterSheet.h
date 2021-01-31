// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryWidget.h"
#include "InventoryInterface.h"
#include "CharacterSheet.generated.h"

class UEquipmentSlot;

/**
 *
 */
UCLASS()
class INVENTORYSYSTEM_API UCharacterSheet : public UInventoryWidget
{
	GENERATED_BODY()

public:
	TArray<UEquipmentSlot*> GetSlots();
	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	void SetSlots(TArray<UEquipmentSlot*> Slots);

private:
	UPROPERTY()
	TArray<UEquipmentSlot*> Slots;
};
