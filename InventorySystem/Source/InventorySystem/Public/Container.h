// Copyright notice

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractInterfaceInventory.h"
#include "SlotStruct.h"
#include "InventoryInterface.h"
#include "Container.generated.h"

UCLASS()
class INVENTORYSYSTEM_API AContainer : public AActor, public IInteractInterfaceInventory, public IInventoryInterface
{
	GENERATED_BODY()

public:
	virtual void Interact(AActor* Interactor) override;
	TArray<FSlotStruct> GetInventory();
	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	bool IsLootable();
	UFUNCTION(BlueprintCallable, Category = "Inventory System")
	void SetInventory(TArray<FSlotStruct> InInventory);

protected:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory System")
	//TArray<FSlotStruct> Inventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory System")
	bool CanBeLooted;

	//virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 NumberOfSlots;

	void PrepareInventory();
};