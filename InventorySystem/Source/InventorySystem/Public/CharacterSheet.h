// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSheet.generated.h"

/**
 *
 */
UCLASS()
class INVENTORYSYSTEM_API UCharacterSheet : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	class UEquipmentSlot* FirstEquipmentSlot;

	TWeakObjectPtr<class UInventoryComponent> InventoryComp;

private:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

};
