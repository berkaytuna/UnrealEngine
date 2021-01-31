// Written by Berkay Tuna, November 2020

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "InventorySystemBPLibrary.h"
#include "ItemStruct.generated.h"

USTRUCT(BlueprintType)
struct FItemStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FName Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TEnumAsByte<EEquipmentSlotType> Subtype;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FName Quality;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	bool IsStackable = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UTexture2D* Thumbnail;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 MaxStackSize = 4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	bool IsConsumable = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	float Durability;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FName Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TSubclassOf<class AItemCpp> Class;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	class USkeletalMesh* Mesh;
};