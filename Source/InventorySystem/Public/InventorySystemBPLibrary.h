// Copyright notice.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventorySystemBPLibrary.generated.h"

class AContainer;

UENUM(BlueprintType)
enum EEquipmentSlotType
{
	Head,
	Chest,
	Legs,
	Gloves,
	Shoes,
	Jewelry,
	OneHanded,
	TwoHanded,
	Consumable,
};

UCLASS()
class UInventorySystemBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	static void Interact(AActor*& InteractedActor, UClass* Class, class AActor* Interactor);
};