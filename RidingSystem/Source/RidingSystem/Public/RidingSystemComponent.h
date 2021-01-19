// Written by Berkay Tuna, October 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RidingSystemComponent.generated.h"

class UBoxComponent;
class EDistmountEnum;
class UAnimMontage;
class UCameraComponent;
class APlayerController;

UENUM()
enum class EMountEnum : uint8
{
	Default,
	MountBegin,
	Ascending,
	MountTurn,
	Riding,
	DismountBegin,
	DismountTurn,
	Descending,
	DismountEnd
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMontagePlayDelegate, FName, NotifyName);

UCLASS(ClassGroup = (RidingSystem), meta = (BlueprintSpawnableComponent, DisplayName = "Riding System"))
class RIDINGSYSTEM_API URidingSystemComponent : public UActorComponent
{

	GENERATED_BODY()


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System")
	bool bCameraTransforms;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System", meta = (editcondition = "bCameraTransforms"))
	float CameraTransformDuration;

	UPROPERTY(BlueprintReadOnly, Category = "Riding System")
	bool bIsRiding;

	UFUNCTION(BlueprintCallable, Category = "Riding")
	void Mount(UAnimMontage* MountMontageLeft, UAnimMontage* MountMontageRight, float InAscendBeginTime, 
		float InAscendDuration, float InSitTime);

	void Dismount(APlayerController* PossesingPlayerController, FTransform ActorTM, USceneComponent* InDismountScene, UAnimMontage* DismountMontage, UCameraComponent* Camera, 
		float InDismountTurnBeginTime, float InDismountTurnDuration, float InGroundedTime);

	URidingSystemComponent();

	UFUNCTION(BlueprintCallable, Category = "Riding")
	void ProvideOverlapInformation(AMountCharacter*& Mount, AActor* OverlappedActor, UPrimitiveComponent* OverlappedComponent, UBoxComponent* BoxCollision);


protected:

	UFUNCTION(Server, Reliable, Category = "Replication")
	void Server_MountBegin(APlayerController* PlayerController, FTransform PlayerCamManagerTM, USceneComponent* InMountScene, UAnimMontage* MountMontage, float InAscendBeginTime,
		float InAscendDuration, float InSitTime);
	virtual void Server_MountBegin_Implementation(APlayerController* PlayerController, FTransform PlayerCamManagerTM, USceneComponent* InMountScene, UAnimMontage* MountMontage, float InAscendBeginTime,
		float InAscendDuration, float InSitTime);
	UFUNCTION(NetMulticast, Reliable, Category = "Replication")
	void Multi_MountBegin(FTransform InActorTM, FTransform PlayerCamManagerTM, USceneComponent* InMountScene, UAnimMontage* MountMontage,
		float InAscendBeginTime, float InAscendDuration, float InSitTime);
	virtual void Multi_MountBegin_Implementation(FTransform InActorTM, FTransform PlayerCamManagerTM, USceneComponent* InMountScene, UAnimMontage* MountMontage,
		float InAscendBeginTime, float InAscendDuration, float InSitTime);

	UFUNCTION()
	void OnMountNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);
	UFUNCTION()
	void OnMountNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);
	UFUNCTION()
	void OnMountMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnDismountNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);
	UFUNCTION()
	void OnDismountNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);
	UFUNCTION()
	void OnDismountMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:

	APlayerController* PossessingPlayerController;

	FVector RiderSceneLoc;
	FRotator RiderSceneRot;

	class UCameraComponent* Camera;

	FVector ChangingVec;
	float ChangingFloat;

	FVector ActorLoc_DuringMounting;
	FVector ActorLoc_Dismount;
	float ActorRotYaw_DuringMounting;
	float ActorRotYaw_Dismount;

	float AscendBeginTime;
	float AscendDuration;
	float SitTime;

	float DismountTurnBeginTime;
	float DismountTurnDuration;
	float GroundedTime;

	class USceneComponent* DismountScene;
	class USceneComponent* MountScene;
	class USceneComponent* RiderScene;

	FTransform ActorTM_MountBegin;
	FTransform ActorTM_AscendBegin;
	FTransform ActorTM_MountTurnBegin;

	FTransform ActorTM_DismountBegin;
	FTransform ActorTM_DescendBegin;

	EMountEnum MountEnum;

	float CapsuleHH;

	int32 MontageInstanceID;

	class AActor* Actor;
	class ACharacter* Char;
	class UCharacterMovementComponent* CharMov;
	UCharacterMovementComponent* MountCharMov;
	class UCapsuleComponent* Capsule;
	class USkeletalMeshComponent* Mesh;
	class UAnimInstance* AnimInstance;

	AActor* MountActor;
	class AMountCharacter* MountChar;
	class UPrimitiveComponent* MountArea;

	//float GetMontagePos(UAnimMontage* Montage);

	void UnbindDelegates();
	bool IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const;
	
	FVector CalcVecThisFrame(FVector TargetVec, FVector InitVec, float EndTime);
	float CalcFloatThisFrame(float TargetFloat, float InitFloat, float EndTime);

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};