// Written by Berkay Tuna, November 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemStruct.h"
#include "InteractInterfaceInventory.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Item.generated.h"

UCLASS()
class INVENTORYSYSTEM_API AItemCpp : public AActor, public IInteractInterfaceInventory
{
	GENERATED_BODY()

	
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory System", meta = (AllowPrivateAccess = "true"))
	//class USceneComponent* Root;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory System", meta = (AllowPrivateAccess = "true"))
	//class USphereComponent* SphereCollision;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory System", meta = (AllowPrivateAccess = "true"))
	//class UStaticMeshComponent* StaticMesh;
	

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Inventory System")
	FItemStruct ItemStruct;

	FItemStruct GetItemStruct();
	virtual void Interact(AActor* Interactor) override;

//public:
	//AItemCpp();
};