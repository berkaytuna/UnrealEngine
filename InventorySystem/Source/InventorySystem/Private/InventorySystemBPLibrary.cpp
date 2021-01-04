// Copyright notice.

#include "InventorySystemBPLibrary.h"
#include "InventorySystem.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InteractInterface.h"
#include "Item.h"

void UInventorySystemBPLibrary::Interact(UClass* ItemBPClass, AActor* Interactor)
{

	TArray<AActor*> OverlappingActors;
	Interactor->GetOverlappingActors(OverlappingActors, ItemBPClass);

	for (int32 i = 0; i < OverlappingActors.Num(); i++)
	{
		AActor* OverlappingActor = OverlappingActors[i];
		//bool DoesImplementInterface = UKismetSystemLibrary::DoesImplementInterface(OverlappingActor, UInteractInterface::StaticClass());

		//if (DoesImplementInterface)
		//{
			AItemCpp* OverlappingItem = Cast<AItemCpp>(OverlappingActor);
			OverlappingItem->Interact(Interactor);
			break;
		//}
	}
}