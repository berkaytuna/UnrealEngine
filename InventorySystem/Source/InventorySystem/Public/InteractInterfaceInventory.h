// Written by Berkay Tuna, November 2020

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "InteractInterfaceInventory.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractInterfaceInventory : public UInterface
{
	GENERATED_BODY()

};

class INVENTORYSYSTEM_API IInteractInterfaceInventory
{
	GENERATED_BODY()

public:

	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory System")
	virtual void Interact(AActor* Interactor) { };
};