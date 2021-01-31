// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotStruct.h"
#include "SlotWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnSlotClicked, USlotWidget*);
DECLARE_DELEGATE_OneParam(FOnSlotAddedToFocusPath, USlotWidget*);
DECLARE_DELEGATE_OneParam(FOnSlotRemovedFromFocusPath, USlotWidget*);

class UButton;
class UImage;
class USlotWindow;
class UInventoryWidget;
class UInventoryComponent;
class UTexture2D;

/**
 *
 */
UCLASS()
class INVENTORYSYSTEM_API USlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnSlotClicked OnSlotClicked;
	FOnSlotAddedToFocusPath OnSlotAddedToFocusPath;
	FOnSlotRemovedFromFocusPath OnSlotRemovedFromFocusPath;

	void DisplayShadow();
	void HideShadow();
	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	void SetButtonAndImages(UButton* NewButton, UImage* NewImage, UImage* NewShadowImage, UTexture2D* EmptySlotTexture);
	UButton* GetButton();
	FSlotStruct GetSlotStruct();
	void SetSlotStruct(FSlotStruct NewSlotStruct);
	UInventoryWidget* GetOwningInventoryWidget();
	UInventoryComponent* GetInventoryComponent();
	void SetOwningInventoryWidget(UInventoryWidget* OwningInventoryWidget);
	void SetInventoryComponent(UInventoryComponent* InventoryComponent);
	virtual void Empty();
	void SetSlotIndex(int32 InSlotIndex);

protected:
	int32 SlotIndex;
	TWeakObjectPtr<UInventoryWidget> OwningInventoryWidget;
	TWeakObjectPtr<UInventoryComponent> InventoryComponent;
	UPROPERTY(BlueprintReadOnly, Category = "Inventory System")
	FSlotStruct SlotStruct;
	TWeakObjectPtr<UButton> Button;
	TWeakObjectPtr<UImage> Image;
	TWeakObjectPtr<UImage> ShadowImage;
	TWeakObjectPtr<USlotWindow> SlotWindow;
	UPROPERTY()
	UTexture2D* EmptySlotTexture;

	UFUNCTION()
	virtual void OnButtonClicked();
	virtual void NativeOnInitialized() override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
};