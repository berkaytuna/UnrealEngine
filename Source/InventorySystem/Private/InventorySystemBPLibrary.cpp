// Copyright notice.

#include "InventorySystemBPLibrary.h"
#include "InventorySystem.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InteractInterfaceInventory.h"
#include "Item.h"
#include "Container.h"
#include "Kismet/KismetSystemLibrary.h"

void UInventorySystemBPLibrary::Interact(AActor*& InteractedActor, UClass* InClass, AActor* Interactor)
{

	TArray<AActor*> OverlappingActors;
	Interactor->GetOverlappingActors(OverlappingActors, InClass);

	for (int32 i = 0; i < OverlappingActors.Num(); i++)
	{
		AActor* OverlappingActor = OverlappingActors[i];
		//bool DoesImplementInterface = UKismetSystemLibrary::DoesImplementInterface(OverlappingActor, UInteractInterface::StaticClass());

		//if (DoesImplementInterface)
		//{
		bool IsActorInteractable = UKismetSystemLibrary::DoesImplementInterface(OverlappingActor, UInteractInterfaceInventory::StaticClass());
		if (IsActorInteractable)
		{
			InteractedActor = OverlappingActor;

			/*AItemCpp* Item = Cast<AItemCpp>(OverlappingActor);
			if (Item != nullptr)
			{
				Item->Interact(Interactor);
				break;
			}
			AContainer* Container = Cast<AContainer>(OverlappingActor);
			if (Container != nullptr)
			{
				Container->Interact(Interactor);
				break;
			}*/
		}
		//}
	}
}