// Copyright

#include "SlotWidget.h"
#include "InventorySystem.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "InventoryComponent.h"
#include "InventoryWidget.h"

void USlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button.IsValid())
	{
		Button.Get()->OnClicked.AddDynamic(this, &USlotWidget::OnButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("SlotWidget - Please use SetButtonAndImage function on initialization to set Button!"));
	}
}

void USlotWidget::Empty()
{
	SlotStruct = {};
	Image.Get()->SetBrushFromTexture(EmptySlotTexture);
}

void USlotWidget::SetSlotIndex(int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
}

UInventoryWidget* USlotWidget::GetOwningInventoryWidget()
{
	return OwningInventoryWidget.Get();
}

UInventoryComponent* USlotWidget::GetInventoryComponent()
{
	return InventoryComponent.Get();
}

void USlotWidget::SetOwningInventoryWidget(UInventoryWidget* InInventoryWidget)
{
	OwningInventoryWidget = InInventoryWidget;
}

void USlotWidget::SetInventoryComponent(UInventoryComponent* InInventoryComponent)
{
	InventoryComponent = InInventoryComponent;
}

FSlotStruct USlotWidget::GetSlotStruct()
{
	return SlotStruct;
}

void USlotWidget::SetSlotStruct(FSlotStruct InSlotStruct)
{
	SlotStruct = InSlotStruct;
	UTexture2D* NewThumbnail = SlotStruct.Item.Thumbnail;
	Image->SetBrushFromTexture(NewThumbnail);
}

void USlotWidget::SetButtonAndImages(UButton* InButton, UImage* InImage, UImage* InShadowImage, UTexture2D* InEmptySlotTexture)
{
	Button = InButton;
	Image = InImage;
	ShadowImage = InShadowImage;
	EmptySlotTexture = InEmptySlotTexture;
}

UButton* USlotWidget::GetButton()
{
	return Button.Get();
}

void USlotWidget::OnButtonClicked()
{
	if (SlotStruct.Quantity > 0)
	{
		OnSlotClicked.ExecuteIfBound(this);
	}
}

void USlotWidget::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);

	OnSlotAddedToFocusPath.ExecuteIfBound(this);
}

void USlotWidget::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);

	OnSlotRemovedFromFocusPath.ExecuteIfBound(this);
}

void USlotWidget::DisplayShadow()
{
	FSlateColor NewTintColor{ FLinearColor(0.0f, 0.0f, 0.0f, 0.35f) };
	ShadowImage.Get()->SetBrushTintColor(NewTintColor);
}

void USlotWidget::HideShadow()
{
	FSlateColor NewTintColor{ FLinearColor(0.0f, 0.0f, 0.0f, 0.0f) };
	ShadowImage.Get()->SetBrushTintColor(NewTintColor);
}