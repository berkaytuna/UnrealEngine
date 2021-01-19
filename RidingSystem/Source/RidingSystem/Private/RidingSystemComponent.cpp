// Written by Berkay Tuna, October 2020

#include "RidingSystemComponent.h"
#include "RidingSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "MountCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"

URidingSystemComponent::URidingSystemComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	MontageInstanceID = INDEX_NONE;

}

/*
float UMantlingSystemComponent::GetMontagePos(UAnimMontage* Montage)
{
	AnimInstance = Mesh->GetAnimInstance();
	float MontagePos = AnimInstance->Montage_GetPosition(Montage);

	return MontagePos;
}
*/

void URidingSystemComponent::UnbindDelegates()
{
	if (AnimInstance)
	{
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &URidingSystemComponent::OnMountNotifyBegin);
		AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &URidingSystemComponent::OnMountNotifyEnd);
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &URidingSystemComponent::OnMountMontageEnded);

		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &URidingSystemComponent::OnDismountNotifyBegin);
		AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &URidingSystemComponent::OnDismountNotifyEnd);
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &URidingSystemComponent::OnDismountMontageEnded);
	}
}

void URidingSystemComponent::ProvideOverlapInformation(AMountCharacter*& Mount, AActor* OverlappedActor, UPrimitiveComponent* OverlappedComponent, UBoxComponent* BoxCollision)
{
	MountArea = OverlappedComponent;
	MountActor = OverlappedActor;
	MountChar = Cast<AMountCharacter>(MountActor);

	if (MountChar)
	{
		UCapsuleComponent* MountCapsule = MountChar->GetCapsuleComponent();

		BoxCollision->IgnoreComponentWhenMoving(MountCapsule, true);

		MountCharMov = MountChar->GetCharacterMovement();
	}

	Mount = MountChar;
}



void URidingSystemComponent::Dismount(APlayerController* InPossessingPlayerController, FTransform ActorTM, USceneComponent* InDismountScene, 
	UAnimMontage* DismountMontage, UCameraComponent* InCamera, float InDismountTurnBeginTime, float InDismountTurnDuration, float InGroundedTime)
{
	if (MountEnum == EMountEnum::Riding)
	{
		PossessingPlayerController = InPossessingPlayerController;

		ActorTM_DismountBegin = ActorTM;

		ActorLoc_Dismount = ActorTM.GetLocation();
		FRotator ActorRot_Dismount = ActorTM.GetRotation().Rotator();
		ActorRotYaw_Dismount = ActorRot_Dismount.Yaw;

		Camera = InCamera;

		DismountTurnBeginTime = InDismountTurnBeginTime;
		DismountTurnDuration = InDismountTurnDuration;
		GroundedTime = InGroundedTime;

		Mesh->IgnoreActorWhenMoving(MountActor, true);
		Capsule->IgnoreActorWhenMoving(MountActor, true);

		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &URidingSystemComponent::OnDismountNotifyBegin);
		AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &URidingSystemComponent::OnDismountNotifyEnd);
		AnimInstance->OnMontageEnded.AddDynamic(this, &URidingSystemComponent::OnDismountMontageEnded);

		AnimInstance->Montage_Play(DismountMontage);
		FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(DismountMontage);
		MontageInstanceID = MontageInstance->GetInstanceID();

		FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		Actor->DetachFromActor(DetachmentRules);

		DismountScene = InDismountScene;

		MountEnum = EMountEnum::DismountBegin;

		bIsRiding = false;

		CharMov->DisableMovement();
		MountCharMov->DisableMovement();
	}
}



void URidingSystemComponent::Mount(UAnimMontage* MountMontageLeft, UAnimMontage* MountMontageRight,
	float InAscendBeginTime, float InAscendDuration, float InSitTime)
{
	if (MountChar && MountEnum == EMountEnum::Default)
	{
		FString MountAreaName;
		MountArea->GetName(MountAreaName);
		bool bMountingFromLeft = MountAreaName.Contains(FString(TEXT("Left")));

		USceneComponent* MountScene_Left;
		USceneComponent* MountScene_Right;
		MountChar->GetMountScene(MountScene_Left, MountScene_Right);
		MountScene = bMountingFromLeft ? MountScene_Left : MountScene_Right;

		UAnimMontage* MountMontage = bMountingFromLeft ? MountMontageLeft : MountMontageRight;

		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

		APlayerCameraManager* PlayerCamManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
		FTransform PlayerCamManagerTM = PlayerCamManager->GetActorTransform();

		Server_MountBegin(PlayerController, PlayerCamManagerTM, MountScene, MountMontage, InAscendBeginTime, InAscendDuration, InSitTime);	
	}
}

void URidingSystemComponent::Server_MountBegin_Implementation(APlayerController* PlayerController, FTransform PlayerCamManagerTM, 
	USceneComponent* InMountScene, UAnimMontage* MountMontage, float InAscendBeginTime, float InAscendDuration, float InSitTime)
{
	PlayerController->Possess(MountChar);

	FTransform ActorTM = Actor->GetActorTransform();
	Multi_MountBegin(ActorTM, PlayerCamManagerTM, InMountScene, MountMontage, InAscendBeginTime, InAscendDuration, InSitTime);
}

void URidingSystemComponent::Multi_MountBegin_Implementation(FTransform InActorTM, FTransform PlayerCamManagerTM, USceneComponent* InMountScene,
	UAnimMontage* MountMontage, float InAscendBeginTime, float InAscendDuration, float InSitTime)
{
	ActorTM_MountBegin = InActorTM;

	ActorLoc_DuringMounting = InActorTM.GetLocation();
	FRotator ActorRot_DuringMounting = InActorTM.GetRotation().Rotator();
	ActorRotYaw_DuringMounting = ActorRot_DuringMounting.Yaw;

	AscendBeginTime = InAscendBeginTime;
	AscendDuration = InAscendDuration;
	SitTime = InSitTime;

	MountChar->Mount(Char, PlayerCamManagerTM);

	MountScene = InMountScene;
	RiderScene = MountChar->RiderLoc;
	RiderSceneLoc = RiderScene->GetComponentLocation();
	RiderSceneRot = RiderScene->GetComponentRotation();

	AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &URidingSystemComponent::OnMountNotifyBegin);
	AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &URidingSystemComponent::OnMountNotifyEnd);
	AnimInstance->OnMontageEnded.AddDynamic(this, &URidingSystemComponent::OnMountMontageEnded);

	AnimInstance->Montage_Play(MountMontage);
	FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MountMontage);
	MontageInstanceID = MontageInstance->GetInstanceID();

	CharMov->DisableMovement();
	MountCharMov->DisableMovement();

	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	bIsRiding = true;

	MountEnum = EMountEnum::MountBegin;
}



FVector URidingSystemComponent::CalcVecThisFrame(FVector TargetVec, FVector InitVec, float EndTime)
{

	FVector VecDiff = TargetVec - InitVec;
	float Distance = VecDiff.Size();
	FVector VecUnit = VecDiff / Distance;

	UAnimMontage* Montage = AnimInstance->GetCurrentActiveMontage();
	float MontagePos = AnimInstance->Montage_GetPosition(Montage);

	float TimeRemaining = EndTime - MontagePos;
	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
	float n = TimeRemaining / DeltaTime;

	float DistanceToTarget = (TargetVec - ChangingVec).Size();

	float Distance_ThisFrame = DistanceToTarget / n;
	Distance_ThisFrame = Distance_ThisFrame > DistanceToTarget ? DistanceToTarget : Distance_ThisFrame;

	FVector VecThisFrame = Distance_ThisFrame * VecUnit;

	return VecThisFrame;

}

float URidingSystemComponent::CalcFloatThisFrame(float TargetFloat, float InitFloat, float EndTime)
{
	float FloatDiff = TargetFloat - InitFloat;
	float Distance = abs(FloatDiff);
	float FloatUnit = FloatDiff / Distance;

	UAnimMontage* Montage = AnimInstance->GetCurrentActiveMontage();
	float MontagePos = AnimInstance->Montage_GetPosition(Montage);

	float TimeRemaining = EndTime - MontagePos;
	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
	float n = TimeRemaining / DeltaTime;

	float DistanceToTarget = abs(TargetFloat - ChangingFloat);

	float Distance_ThisFrame = DistanceToTarget / n;
	Distance_ThisFrame = Distance_ThisFrame > DistanceToTarget ? DistanceToTarget : Distance_ThisFrame;

	float FloatThisFrame = Distance_ThisFrame * FloatUnit;

	return FloatThisFrame;
}



bool URidingSystemComponent::IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const
{
	return ((MontageInstanceID != INDEX_NONE) && (BranchingPointNotifyPayload.MontageInstanceID == MontageInstanceID));
}



void URidingSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	Actor = GetOwner();
	Char = Cast<ACharacter>(Actor);
	Mesh = Char ->GetMesh();
	Capsule = Char->GetCapsuleComponent();
	CharMov = Char->GetCharacterMovement();
	CapsuleHH = Capsule->GetScaledCapsuleHalfHeight();
	
	AnimInstance = Mesh->GetAnimInstance();
}

void URidingSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (MountEnum != EMountEnum::Default)
	{
		float EndTime;

		FVector TargetLoc;
		float TargetLocZ;

		FVector InitLoc;
		float InitLocZ;

		FVector DismountSceneLoc;
		//FVector RiderSceneLoc;

		FRotator TargetRot;
		float TargetRotYaw;
		FRotator InitRot;
		float InitRotYaw;

		FVector LocThisFrame;
		float LocZThisFrame;
		float RotYawThisFrame;

		FRotator ActorRot;
		FRotator ActorRot_DuringMounting;
		FRotator ActorRot_Dismount;

		FVector ActorScale;

		FString RiderSceneName;


		switch (MountEnum)
		{

		case EMountEnum::MountBegin:

			EndTime = AscendBeginTime;

			TargetLoc = MountScene->GetComponentLocation();
			TargetLoc.Z += CapsuleHH;
			InitLoc = ActorTM_MountBegin.GetLocation();

			TargetRot = MountScene->GetComponentRotation();
			TargetRotYaw = TargetRot.Yaw;
			InitRot = ActorTM_MountBegin.GetRotation().Rotator();
			InitRotYaw = InitRot.Yaw;

			if (TargetRotYaw - InitRotYaw > 180.0f)
			{
				TargetRotYaw -= 360.0f;
			}
			else if (InitRotYaw - TargetRotYaw > 180.0f)
			{
				TargetRotYaw += 360.0f;
			}

			ChangingVec = ActorLoc_DuringMounting;
			LocThisFrame = CalcVecThisFrame(TargetLoc, InitLoc, EndTime);
			ActorLoc_DuringMounting += LocThisFrame;

			ChangingFloat = ActorRotYaw_DuringMounting;
			RotYawThisFrame = CalcFloatThisFrame(TargetRotYaw, InitRotYaw, EndTime);
			ActorRotYaw_DuringMounting += RotYawThisFrame;
			ActorRot = Actor->GetActorRotation();
			ActorRot_DuringMounting = FRotator(ActorRot.Pitch, ActorRotYaw_DuringMounting, ActorRot.Roll);

			Actor->SetActorLocation(ActorLoc_DuringMounting);
			Actor->SetActorRotation(ActorRot_DuringMounting);

			ActorScale = Actor->GetActorScale();
			ActorTM_AscendBegin = FTransform(ActorRot_DuringMounting, ActorLoc_DuringMounting, ActorScale);

			//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString::Printf(TEXT("%f"), ActorRotYaw_DuringMounting));
			//UE_LOG(LogTemp, Warning, TEXT("%f"), ActorRotYaw_DuringMounting);

			break;


		case EMountEnum::Ascending:
			
			EndTime = AscendBeginTime + AscendDuration;		

			TargetLocZ = RiderScene->GetComponentLocation().Z;
			InitLocZ = ActorTM_AscendBegin.GetLocation().Z;

			ChangingFloat = ActorLoc_DuringMounting.Z;
			LocZThisFrame = CalcFloatThisFrame(TargetLocZ, InitLocZ, EndTime);
			ActorLoc_DuringMounting.Z += LocZThisFrame;
			//ActorLoc_DuringMounting.X = InitLoc.X;
			//ActorLoc_DuringMounting.Y = InitLoc.Y;

			Actor->SetActorLocation(ActorLoc_DuringMounting);

			ActorRot = Actor->GetActorRotation();
			ActorScale = Actor->GetActorScale();
			ActorTM_MountTurnBegin = FTransform(ActorRot, ActorLoc_DuringMounting, ActorScale);

			ActorRotYaw_DuringMounting = ActorRot.Yaw;
			
			break;


		case EMountEnum::MountTurn:
			
			EndTime = SitTime;

			TargetLoc = RiderSceneLoc;
			InitLoc = ActorTM_MountTurnBegin.GetLocation();

			TargetRot = RiderSceneRot;
			TargetRotYaw = TargetRot.Yaw;
			InitRot = ActorTM_MountTurnBegin.GetRotation().Rotator();
			InitRotYaw = InitRot.Yaw;

			if (TargetRotYaw - InitRotYaw > 180.0f)
			{
				TargetRotYaw -= 360.0f;
			}
			else if (InitRotYaw - TargetRotYaw > 180.0f)
			{
				TargetRotYaw += 360.0f;
			}

			ChangingVec = ActorLoc_DuringMounting;
			LocThisFrame = CalcVecThisFrame(TargetLoc, InitLoc, EndTime);
			ActorLoc_DuringMounting += LocThisFrame;

			ChangingFloat = ActorRotYaw_DuringMounting;
			RotYawThisFrame = CalcFloatThisFrame(TargetRotYaw, InitRotYaw, EndTime);
			ActorRotYaw_DuringMounting += RotYawThisFrame;
			
			ActorRot = Actor->GetActorRotation();
			ActorRot_DuringMounting = FRotator(ActorRot.Pitch, ActorRotYaw_DuringMounting, ActorRot.Roll);

			Actor->SetActorLocation(ActorLoc_DuringMounting);
			Actor->SetActorRotation(ActorRot_DuringMounting);

			RiderSceneName = RiderScene->GetName();

			//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString::Printf(TEXT("%f"), ActorRotYaw_DuringMounting));
			//UE_LOG(LogTemp, Warning, TEXT("%f"), ActorRotYaw_DuringMounting);
			
			break;


		case EMountEnum::DismountTurn:

			EndTime = DismountTurnBeginTime + DismountTurnDuration;

			DismountSceneLoc = DismountScene->GetComponentLocation();
			RiderSceneLoc = RiderScene->GetComponentLocation();

			TargetLoc = FVector(DismountSceneLoc.X, DismountSceneLoc.Y, RiderSceneLoc.Z);
			InitLoc = ActorTM_DismountBegin.GetLocation();

			TargetRot = DismountScene->GetComponentRotation();
			TargetRotYaw = TargetRot.Yaw;
			InitRot = ActorTM_DismountBegin.GetRotation().Rotator();
			InitRotYaw = InitRot.Yaw;

			if (TargetRotYaw - InitRotYaw > 180.0f)
			{
				TargetRotYaw -= 360.0f;
			}
			else if (InitRotYaw - TargetRotYaw > 180.0f)
			{
				TargetRotYaw += 360.0f;
			}

			ChangingVec = ActorLoc_Dismount;
			LocThisFrame = CalcVecThisFrame(TargetLoc, InitLoc, EndTime);
			ActorLoc_Dismount += LocThisFrame;

			ChangingFloat = ActorRotYaw_Dismount;
			RotYawThisFrame = CalcFloatThisFrame(TargetRotYaw, InitRotYaw, EndTime);
			ActorRotYaw_Dismount += RotYawThisFrame;

			ActorRot = Actor->GetActorRotation();
			ActorRot_Dismount = FRotator(ActorRot.Pitch, ActorRotYaw_Dismount, ActorRot.Roll);

			Actor->SetActorLocation(ActorLoc_Dismount);
			Actor->SetActorRotation(ActorRot_Dismount);

			ActorScale = Actor->GetActorScale();
			ActorTM_DescendBegin = FTransform(ActorRot_Dismount, ActorLoc_Dismount, ActorScale);

			//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString::Printf(TEXT("%f"), ActorRotYaw_Dismount));
			//UE_LOG(LogTemp, Warning, TEXT("%f"), ActorRotYaw_Dismount);

			break;


		case EMountEnum::Descending:

			EndTime = GroundedTime;

			DismountSceneLoc = DismountScene->GetComponentLocation();;
			DismountSceneLoc.Z += CapsuleHH;

			TargetLoc = DismountSceneLoc;
			InitLoc = ActorTM_DescendBegin.GetLocation();

			TargetRot = DismountScene->GetComponentRotation();
			TargetRotYaw = TargetRot.Yaw;
			InitRot = ActorTM_DescendBegin.GetRotation().Rotator();
			InitRotYaw = InitRot.Yaw;

			if (TargetRotYaw < 0.0f && InitRotYaw > 0.0f)
			{
				if (TargetRotYaw < 0.0f)
				{
					TargetRotYaw += 360.0f;
				}
			}

			ChangingVec = ActorLoc_Dismount;
			LocThisFrame = CalcVecThisFrame(TargetLoc, InitLoc, EndTime);
			ActorLoc_Dismount += LocThisFrame;

			ChangingFloat = ActorRotYaw_Dismount;
			RotYawThisFrame = CalcFloatThisFrame(TargetRotYaw, InitRotYaw, EndTime);
			ActorRotYaw_Dismount += RotYawThisFrame;

			ActorRot = Actor->GetActorRotation();
			ActorRot_Dismount = FRotator(ActorRot.Pitch, ActorRotYaw_Dismount, ActorRot.Roll);

			Actor->SetActorLocation(ActorLoc_Dismount);
		//	Actor->SetActorRotation(ActorRot_Dismount);

			break;
		}
	}
}



void URidingSystemComponent::OnMountNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		MountEnum = EMountEnum::Ascending;
	}
}

void URidingSystemComponent::OnMountNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		MountEnum = EMountEnum::MountTurn;
	}
}

void URidingSystemComponent::OnMountMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		MountEnum = EMountEnum::Riding;

		FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);

		Actor->AttachToComponent(RiderScene, AttachmentRules);

		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

		MountCharMov->SetMovementMode(MOVE_Walking);
	}

	UnbindDelegates();
}



void URidingSystemComponent::OnDismountNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		MountEnum = EMountEnum::DismountTurn;

		
	}
}

void URidingSystemComponent::OnDismountNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		MountEnum = EMountEnum::Descending;

		if (Actor->HasAuthority())
		{
			APawn* Pawn = Char;

			PossessingPlayerController->Possess(Pawn);
		}

		if (bCameraTransforms)
		{
			APlayerCameraManager* PlayerCamManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
			FTransform PlayerCamManagerTM = PlayerCamManager->GetActorTransform();

			Camera->SetWorldTransform(PlayerCamManagerTM, false, nullptr, ETeleportType::TeleportPhysics);

			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			UKismetSystemLibrary::MoveComponentTo(Camera, FVector::ZeroVector, FRotator::ZeroRotator, false, false, CameraTransformDuration, false,
				EMoveComponentAction::Type::Move, LatentInfo);
		}
	}
}

void URidingSystemComponent::OnDismountMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		MountEnum = EMountEnum::Default;

		Mesh->IgnoreActorWhenMoving(MountActor, false);
		Capsule->IgnoreActorWhenMoving(MountActor, false);

		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

		CharMov->SetMovementMode(MOVE_Walking);
		MountCharMov->SetMovementMode(MOVE_Walking);
	}

	UnbindDelegates();
}