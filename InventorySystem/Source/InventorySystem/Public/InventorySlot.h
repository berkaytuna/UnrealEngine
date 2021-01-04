// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotStruct.h"
#include "InventorySlot.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:
	int32 SlotIndex;
	UPROPERTY(BlueprintReadWrite, Category = "Inventory System")
	FSlotStruct SlotStruct;
	TWeakObjectPtr<class UInventoryComponent> InventoryComp;
	UPROPERTY(BlueprintReadWrite, Category = "Inventory System")
	class UButton* Button;
};
