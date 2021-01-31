// Copyright notice

#include "Container.h"
#include "InventorySystem.h"
#include "InventoryComponent.h"

/*void AContainer::BeginPlay()
{
	PrepareInventory();

	Super::BeginPlay();
}*/

void AContainer::PrepareInventory()
{
	Inventory.Empty(NumberOfSlots);
	Inventory.AddZeroed(NumberOfSlots);
}

TArray<FSlotStruct> AContainer::GetInventory()
{
	return Inventory;
}

void AContainer::SetInventory(TArray<FSlotStruct> InInventory)
{
	Inventory = InInventory;
}

bool AContainer::IsLootable()
{
	return CanBeLooted;
}

void AContainer::Interact(AActor* Interactor)
{
	/*if (CanBeLooted)
	{
		UActorComponent* ActorComponent = Interactor->GetComponentByClass(UInventoryComponent::StaticClass());
		UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(ActorComponent);

		InventoryComponent->SetLootWindow(Inventory);
	}*/
}