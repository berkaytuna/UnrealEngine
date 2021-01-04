// Copyright notice.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventorySystemBPLibrary.generated.h"

UCLASS()
class UInventorySystemBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	static void Interact(UClass* ItemBPClass, class AActor* Interactor);
};