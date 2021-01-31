// Copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotStruct.h"
#include "InventoryComponent.h"
#include "InfoWindow.generated.h"

UCLASS()
class INVENTORYSYSTEM_API UInfoWindow : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSlotStruct(FSlotStruct InSlotStruct);
	void SetClickReason(EClickReason NewClickReason);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Inventory System")
	FSlotStruct SlotStruct;
	UPROPERTY(BlueprintReadOnly, Category = "Inventory System")
	TEnumAsByte<EClickReason> ClickReason;
};