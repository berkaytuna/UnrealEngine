// Written by Berkay Tuna, November 2020

#include "Item.h"
#include "InventorySystem.h"
#include "InventoryComponent.h"

/*AItemCpp::AItemCpp()
{	
	
	//Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//Root->SetupAttachment(RootComponent);

	//SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	//SphereCollision->SetupAttachment(RootComponent);

	//StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	//StaticMesh->SetupAttachment(RootComponent);
	
}*/

void AItemCpp::Interact(AActor* Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("Item interacted!"));
	
	UActorComponent* ActorComp = Interactor->GetComponentByClass(UInventoryComponent::StaticClass());
	UInventoryComponent* InventoryComp = Cast<UInventoryComponent>(ActorComp);

	FSlotStruct SlotStruct = FSlotStruct{ ItemStruct, 1 };
	bool bSuccess = InventoryComp->AddToInventory(SlotStruct);

	if (bSuccess)
	{
		this->Destroy();
	}
}