// Written by Berkay Tuna, June 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PassiveRagdollStruct.h"
#include "PassiveRagdollComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRagdollEndDelegate, FPassiveRagdollStruct, Struct, UAnimInstance*, AnimInstance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGetUpDelegate, FPassiveRagdollStruct, Struct, UAnimInstance*, AnimInstance);

class ACharacter;
class UCapsuleComponent;
class UCharacterMovementComponent;

UCLASS(ClassGroup = (PassiveRagdoll), meta = (BlueprintSpawnableComponent, DisplayName = "Passive Ragdoll"))
class PASSIVERAGDOLL_API UPassiveRagdollComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	float MeshSpeed;
	UPROPERTY(EditAnywhere, Category = "Ragdoll Behaviour")
	bool bGetUpSelf;
	bool bCanEnterRagdoll;
	int32 RagdollEndCounter;
	int32 GetUpCounter;
	int32 GetUpFrames;

	// initial values
	UPROPERTY(EditAnywhere, Category = "Bone Reference")
	FName Root;
	UPROPERTY(EditAnywhere, Category = "Bone Reference")
	FName Pelvis;
	UPROPERTY(EditAnywhere, Category = "Bone Reference")
	FName LeftFoot;
	UPROPERTY(EditAnywhere, Category = "Bone Reference")
	FName RightFoot;

	UPROPERTY(EditAnywhere, Category = "Animation", meta = (DisplayName = "Ragdoll Pose Snapshot Name"))
	FName PoseSnapshot;
	UPROPERTY(EditAnywhere, Category = "Animation", meta = (DisplayName = "Get Up Montage Forwards"))
	UAnimMontage* Montage_F;
	UPROPERTY(EditAnywhere, Category = "Animation", meta = (DisplayName = "Get Up Montage Backwards"))
	UAnimMontage* Montage_B;

	UPROPERTY(EditAnywhere, Category = "Calculations")
	float AngleThreshold;
	UPROPERTY(EditAnywhere, Category = "Calculations")
	float SpeedThreshold;
	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (DisplayName = "Ragdoll Recover Speed"))
	float RecoverSpeed;
	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (DisplayName = "Line Trace Apply Distance"))
	float ApplyDistance;
	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (DisplayName = "Ragdoll Line Trace Multiplier"))
	float Multiplier;
	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (DisplayName = "Capsule Offset Correction"))
	float OffsetCorrection;

	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (DisplayName = "Actor Location Interpolation Alpha"))
	float ActorLocAlpha;
	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (DisplayName = "Mesh Location Interpolation Alpha"))
	float MeshLocAlpha;
	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (DisplayName = "Mesh Rotation Interpolation Alpha"))
	float MeshRotAlpha;

	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (DisplayName = "Location Update Frequency in Ragdoll"))
	int32 UpdateFrequency;

	// references
	AActor* Actor;
	ACharacter* Char;
	UCapsuleComponent* Capsule;
	UCharacterMovementComponent* CharMov;
	USkeletalMeshComponent* Mesh;

	bool bInRagdoll;
	bool bRagdollEnd;
	bool bCanGetUp;

	int32 i;

	FTransform RagdollEnd_RootTM;

	UPROPERTY(BlueprintAssignable, Category = "PassiveRagdoll")
	FRagdollEndDelegate RagdollEndDelegate;
	UPROPERTY(BlueprintAssignable, Category = "PassiveRagdoll")
	FGetUpDelegate GetUpDelegate;

	// Sets default values for this component's properties
	UPassiveRagdollComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Passive Ragdoll")
	void RagdollEnd();
	UFUNCTION(BlueprintCallable, Category = "Passive Ragdoll")
	void GetUp();

protected:

	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, Category = "Replication")
	void Server_RagdollStart();
	virtual void Server_RagdollStart_Implementation();
	UFUNCTION(NetMulticast, Reliable, Category = "Replication")
	void Multicast_RagdollStart();
	virtual void Multicast_RagdollStart_Implementation();

	UFUNCTION(Server, Reliable, Category = "Replication")
	void Server_RagdollUpdate(FVector InActorLoc, FRotator InActorRot, FVector InMeshLoc, FQuat InMeshQuat);
	virtual void Server_RagdollUpdate_Implementation(FVector InActorLoc, FRotator InActorRot, FVector InMeshLoc, FQuat InMeshQuat);
	UFUNCTION(NetMulticast, Reliable, Category = "Replication")
	void Multicast_RagdollUpdate(FVector InActorLoc, FRotator InActorRot, FVector InMeshLoc, FQuat InMeshQuat);
	virtual void Multicast_RagdollUpdate_Implementation(FVector InActorLoc, FRotator InActorRot, FVector InMeshLoc, FQuat InMeshQuat);

	UFUNCTION(Server, Reliable, Category = "Replication")
	void Server_CanEnterRagdollEnd();
	virtual void Server_CanEnterRagdollEnd_Implementation();
	UFUNCTION(NetMulticast, Reliable, Category = "Replication")
	void Multicast_CanEnterRagdollEnd();
	virtual void Multicast_CanEnterRagdollEnd_Implementation();
	UFUNCTION(Server, Reliable, Category = "Replication")
	void Server_RagdollEnd();
	virtual void Server_RagdollEnd_Implementation();
	UFUNCTION(NetMulticast, Reliable, Category = "Replication")
	void Multicast_RagdollEnd();
	virtual void Multicast_RagdollEnd_Implementation();

	UFUNCTION(Server, Reliable, Category = "Replication")
	void Server_GetUp(UAnimMontage* GetUpMontage);
	virtual void Server_GetUp_Implementation(UAnimMontage* GetUpMontage);
	UFUNCTION(NetMulticast, Reliable, Category = "Replication")
	void Multicast_GetUp(UAnimMontage* GetUpMontage);
	virtual void Multicast_GetUp_Implementation(UAnimMontage* GetUpMontage);

private:

	void LineTrace_Foot(bool& Hit, float& HitAngle, FName SocketName);
	void LineTrace_Actor(bool& Hit, FVector& HitLoc, FVector& HitNormal);
};