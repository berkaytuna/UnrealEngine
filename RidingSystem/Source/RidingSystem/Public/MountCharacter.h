// Written by Berkay Tuna, October 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MountCharacter.generated.h"

class URidingSystemComponent;
class APlayerController;
class APlayerCameraManager;

UENUM()
enum class EDismountEnum : uint8
{
	Left,
	Right
};

UCLASS(config=Game)
class AMountCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scene", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* MountLoc_Left;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scene", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* MountLoc_Right;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* MountArea_Left;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* MountArea_Right;


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System")
	bool bCameraTransforms;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System", meta = (editcondition = "bCameraTransforms"))
	float CameraTransformDuration;

	UPROPERTY(BlueprintReadOnly, Category = "Default")
	class AActor* RiderActor;

	UPROPERTY(BlueprintReadOnly, Category = "Default")
	bool bIsRiding;

	UPROPERTY(BlueprintReadWrite, Category = "Default")
	USceneComponent* RiderLoc;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	void GetMountScene(USceneComponent*& MountScene_Left, USceneComponent*& MountScene_Right);

	void Mount(ACharacter* InChar, FTransform PlayerCamManagerTM);
	UFUNCTION(BlueprintCallable, Category = "Riding")
	void Dismount(URidingSystemComponent* RidingSystemComp, UCameraComponent* RiderCamera, UAnimMontage* DismountMontage, EDismountEnum DismountFrom,
		float DismountTurnBeginTime, float DismountTurnDuration, float GroundedTime);

	AMountCharacter();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	UFUNCTION(Server, Reliable, Category = "Replication")
	void Server_Dismount(APlayerController* PossessingPlayerController, USceneComponent* InDismountScene, URidingSystemComponent* RidingSystemComp,
		UCameraComponent* RiderCamera, UAnimMontage* DismountMontage, float DismountTurnBeginTime,
		float DismountTurnDuration, float GroundedTime);
	virtual void Server_Dismount_Implementation(APlayerController* PossessingPlayerController, USceneComponent* InDismountScene, URidingSystemComponent* RidingSystemComp,
		UCameraComponent* RiderCamera, UAnimMontage* DismountMontage, float DismountTurnBeginTime,
		float DismountTurnDuration, float GroundedTime);
	UFUNCTION(NetMulticast, Reliable, Category = "Replication")
	void Multi_Dismount(APlayerController* PossessingPlayerController, FTransform RiderActorTransform, USceneComponent* InDismountScene, URidingSystemComponent* RidingSystemComp,
		UCameraComponent* RiderCamera, UAnimMontage* DismountMontage, float DismountTurnBeginTime,
		float DismountTurnDuration, float GroundedTime);
	virtual void Multi_Dismount_Implementation(APlayerController* PossessingPlayerController, FTransform RiderActorTransform, USceneComponent* InDismountScene, URidingSystemComponent* RidingSystemComp,
		UCameraComponent* RiderCamera, UAnimMontage* DismountMontage, float DismountTurnBeginTime,
		float DismountTurnDuration, float GroundedTime);

private:

	class ACharacter* RiderChar;

	class USkeletalMeshComponent* Mesh;
	class UCapsuleComponent* Capsule;

	virtual void BeginPlay() override;
};

