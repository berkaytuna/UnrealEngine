// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/InputSettings.h"
#include "InventoryWindow.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UInventoryWindow : public UUserWidget
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, noclear, Category = "Inventory System")
	UClass* InventorySlotBPClass;

	TWeakObjectPtr<class UInventoryComponent> InventoryComp;
	TWeakObjectPtr<class UInventorySlot> FirstInventorySlot;
	UPROPERTY(BlueprintReadWrite, Category = "Inventory System")
	class UGridPanel* InventoryGrid;

	virtual void NativeConstruct() override;

private:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	//virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
};
