// Written by Berkay Tuna, June 2020

#include "PassiveRagdollComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

UPassiveRagdollComponent::UPassiveRagdollComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	AngleThreshold = 40.0f;
	SpeedThreshold = 1200.0f;	
	RecoverSpeed = 13.5f;
	ApplyDistance = 60.0f;
	Multiplier = 1.5f;
	OffsetCorrection = 2.0f;

	ActorLocAlpha = 0.15f;
	MeshLocAlpha = 0.1f;
	MeshRotAlpha = 0.1f;
	
	UpdateFrequency = 5;

	MeshSpeed = 9000.0f;
	GetUpFrames = 120;

}

void UPassiveRagdollComponent::BeginPlay()
{
	Super::BeginPlay();

	Actor = GetOwner();
	Char = Cast<ACharacter>(Actor);
	Capsule = Char->GetCapsuleComponent();
	CharMov = Char->GetCharacterMovement();
	Mesh = Char->GetMesh();
}

void UPassiveRagdollComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bGetUpSelf)
	{
		float Speed = CharMov->Velocity.Size();
		bool bIsInAir = CharMov->IsFalling();

		if (Actor->HasAuthority())
		{
			if (Speed > 900.0f)
			{
				Multicast_RagdollStart();
			}

			if (bInRagdoll)
			{
				FVector NewActorLoc = Mesh->GetSocketLocation(TEXT("Pelvis"));
				FVector ActorLoc = Actor->GetActorLocation();
				FRotator ActorRot = Actor->GetActorRotation();
				FRotator MeshRot = Mesh->GetComponentRotation();
				float CapsuleHH = Capsule->GetUnscaledCapsuleHalfHeight();

				NewActorLoc.Z += CapsuleHH;

				ActorLoc = ActorLoc + (NewActorLoc - ActorLoc) * 0.1f;

				FRotator NewMeshRot = FRotator(MeshRot.Pitch, ActorRot.Yaw, MeshRot.Roll);


				Actor->SetActorLocation(ActorLoc);
				//Mesh->SetWorldRotation(NewMeshRot, false, nullptr, ETeleportType::TeleportPhysics);

				float MeshSpeed_Real = Mesh->GetComponentVelocity().Size();
				MeshSpeed = MeshSpeed_Real + (MeshSpeed - MeshSpeed_Real) * 0.1f;



				if (MeshSpeed < RecoverSpeed)
				{
					//FRotator ActorRot = Actor->GetActorRotation();

					//Actor->SetActorRotation(ActorRot);

					//FRotator PelvisRot = Mesh->GetSocketRotation(TEXT("Pelvis"));
					//float PelvisRotYaw = PelvisRot.Yaw;

					Multicast_RagdollEnd();
				}


			}
			else
			{
				MeshSpeed = 9000.0f;
			}

			/*
			if (MeshSpeed < RecoverSpeed)
			{
				//FRotator ActorRot = Actor->GetActorRotation();

				//Actor->SetActorRotation(ActorRot);

				FRotator PelvisRot = Mesh->GetSocketRotation(TEXT("Pelvis"));
				float PelvisRotYaw = PelvisRot.Yaw;


				Multicast_RagdollEnd();
			}
			*/

		}

		if (!bIsInAir && bCanEnterRagdoll)
		{
			bCanEnterRagdoll = false;

			Mesh->SetCollisionObjectType(ECC_PhysicsBody);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Mesh->SetAllBodiesBelowSimulatePhysics(Pelvis, true, true);
			Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CharMov->SetMovementMode(MOVE_None);
			//Actor->SetReplicateMovement(false);
			bInRagdoll = true;
		}

		if (bInRagdoll && !Actor->HasAuthority())
		{
			RagdollEndCounter++;

			FVector NewMeshLoc = Actor->GetActorLocation();
			FVector MeshLoc = Mesh->GetComponentLocation();
			float CapsuleHH = Capsule->GetUnscaledCapsuleHalfHeight();

			FVector PelvisLoc = Mesh->GetSocketLocation(TEXT("Pelvis"));

			NewMeshLoc.X -= PelvisLoc.X - MeshLoc.X;
			NewMeshLoc.Y -= PelvisLoc.Y - MeshLoc.Y;
			NewMeshLoc.Z -= CapsuleHH;

			MeshLoc = MeshLoc + (NewMeshLoc - MeshLoc) * MeshLocAlpha;

			if (RagdollEndCounter > 1)
			{
				RagdollEndCounter = 0;

				Mesh->SetWorldLocation(MeshLoc, false, nullptr, ETeleportType::TeleportPhysics);
			}
		}

		if (Char->IsLocallyControlled())
		{

			if (bCanGetUp)
			{
				GetUpCounter++;

				if (GetUpCounter > GetUpFrames)
				{
					GetUpCounter = 0;

					GetUp();
				}
			}
		}
	}
	else
	{
		if (Char->IsLocallyControlled()) { // Only owning client enters here, it will pass any needed info to the server
			TEnumAsByte<enum EMovementMode> Mode = CharMov->MovementMode;

			if (Mode == 1) { // walking movement mode
				struct FLeft { FName Name; bool Hit; float HitAngle; } Left;
				struct FRight { FName Name; bool Hit; float HitAngle; } Right;
				bool Hit;

				LineTrace_Foot(Left.Hit, Left.HitAngle, LeftFoot);
				LineTrace_Foot(Right.Hit, Right.HitAngle, RightFoot);

				Hit = Left.Hit || Right.Hit ? true : false;

				// Ragdoll Start (Hit Angle)
				if ((Left.HitAngle > AngleThreshold) && (Right.HitAngle > AngleThreshold)) {
					Server_RagdollStart();
				}
			}
			else if ((Mode == 3) || bInRagdoll) { // "falling" and "in ragdoll" share same line trace
				bool Hit;
				FVector HitLoc;
				FVector HitNormal;

				LineTrace_Actor(Hit, HitLoc, HitNormal);

				float Speed = Char->GetVelocity().Size();

				// Ragdoll Start (Speed)
				if (Mode == 3 && Hit && Speed > SpeedThreshold) { // if hit to the ground faster than a certain speed, begin ragdoll			
					Server_RagdollStart();
				}

				// Ragdoll Update		
				if (bInRagdoll && (i % UpdateFrequency == 0)) {
					FVector ActorLoc = Actor->GetActorLocation();
					FRotator ActorRot = Actor->GetActorRotation();
					FVector NewActorLoc;
					FRotator NewActorRot;

					FVector PelvisLoc = Mesh->GetSocketLocation(Pelvis);
					FRotator PelvisRot = Mesh->GetSocketRotation(Pelvis);

					float CapsuleHH = Capsule->GetUnscaledCapsuleHalfHeight();
					float CapsuleRadius = Capsule->GetUnscaledCapsuleRadius();

					if (Hit) { // if close to the ground (lower interpolation alpha to avoid stutter)
						float Angle1 = fabsf(FGenericPlatformMath::Atan2(HitNormal.X, HitNormal.Z));
						float Angle2 = fabsf(FGenericPlatformMath::Atan2(HitNormal.Y, HitNormal.Z));
						float AngleMax = Angle1 > Angle2 ? Angle1 : Angle2;
						float SlopeCorrection = CapsuleRadius * (1 / cos(AngleMax) - 1); // capsule radius causing offset on slopes, resulting sharp change of actor location
																						 //	(so camera movement) when returning to walking mode after ragdoll recovery.
						NewActorLoc = FVector(PelvisLoc.X, PelvisLoc.Y, HitLoc.Z + CapsuleHH + OffsetCorrection + SlopeCorrection);
						ActorLoc = ActorLoc + ActorLocAlpha * (NewActorLoc - ActorLoc);

						float NewActorYaw = PelvisRot.Roll > 0.0f ? PelvisRot.Yaw : PelvisRot.Yaw - 180;
						NewActorRot = FRotator(ActorRot.Pitch, NewActorYaw, ActorRot.Roll);
						ActorRot = NewActorRot;
					}
					else { // if falling, dont use any interpolation, otherwise camera will not catch up with mesh
					//	Alpha = Alpha + 0.1f * (1.0f - Alpha); // for smooth camera pass from "interpolation" to "no interpolation"
						NewActorLoc = PelvisLoc + FVector(0.0f, 0.0f, CapsuleHH);
						ActorLoc = ActorLoc + ActorLocAlpha * (NewActorLoc - ActorLoc);
					}

					FVector MeshLoc = Mesh->GetComponentLocation();
					FQuat MeshQuat = Mesh->GetComponentTransform().GetRotation();

					Server_RagdollUpdate(ActorLoc, ActorRot, MeshLoc, MeshQuat);
				}
				i++;

				// Ragdoll End Permission
				float SpeedInRagdoll = Mesh->GetComponentVelocity().Size();

				if (bInRagdoll && (Speed == 0.0f) && (SpeedInRagdoll <= RecoverSpeed)) {
					Server_CanEnterRagdollEnd();
				}
			}
		}
	}

}	

void UPassiveRagdollComponent::LineTrace_Foot(bool& Hit, float& HitAngle, FName Foot)
{
	FVector ActorLoc = Actor->GetActorLocation();
	float CapsuleHH = Capsule->GetUnscaledCapsuleHalfHeight();

	FVector SocketLoc = Mesh->GetSocketLocation(Foot);

	ECollisionChannel CollisionChannel = UCollisionProfile::Get()->ConvertToCollisionChannel(true, (int32)TraceTypeQuery1);

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(Char);

	FHitResult HitResult;
	FCollisionQueryParams Params(FName("Line Trace Single"), SCENE_QUERY_STAT_ONLY(KismetTraceUtils), false);
	Params.bReturnPhysicalMaterial = true;
	Params.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable; // Ask for face index, as long as we didn't disable globally
	Params.AddIgnoredActors(IgnoredActors);

	FVector Start = FVector(SocketLoc.X, SocketLoc.Y, ActorLoc.Z);
	FVector End = FVector(SocketLoc.X, SocketLoc.Y, ActorLoc.Z - CapsuleHH - ApplyDistance);

	FGenericPhysicsInterface::RaycastSingle(Char->GetWorld(), HitResult, Start, End, CollisionChannel, Params, FCollisionResponseParams::DefaultResponseParam, FCollisionObjectQueryParams::DefaultObjectQueryParam);

	Hit = HitResult.bBlockingHit;

	FVector_NetQuantizeNormal HitNormal = HitResult.Normal;

	float HitAngleX = fabsf(180 / PI * FGenericPlatformMath::Atan2(HitNormal.X, HitNormal.Z));
	float HitAngleY = fabsf(180 / PI * FGenericPlatformMath::Atan2(HitNormal.Y, HitNormal.Z));

	HitAngle = HitAngleX > HitAngleY ? HitAngleX : HitAngleY;
}
void UPassiveRagdollComponent::LineTrace_Actor(bool& Hit, FVector& HitLoc, FVector& HitNormal)
{
	FVector ActorLoc = Actor->GetActorLocation();
	float CapsuleHH = Capsule->GetUnscaledCapsuleHalfHeight();

	ECollisionChannel CollisionChannel = UCollisionProfile::Get()->ConvertToCollisionChannel(true, (int32)TraceTypeQuery1);

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(Char);

	FHitResult HitResult;
	FCollisionQueryParams Params(FName("Line Trace Single"), SCENE_QUERY_STAT_ONLY(KismetTraceUtils), false);
	Params.bReturnPhysicalMaterial = true;
	Params.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable; // Ask for face index, as long as we didn't disable globally
	Params.AddIgnoredActors(IgnoredActors);

	FVector Start = ActorLoc;
	FVector End = FVector(ActorLoc.X, ActorLoc.Y, ActorLoc.Z - Multiplier * CapsuleHH);

	FGenericPhysicsInterface::RaycastSingle(Char->GetWorld(), HitResult, Start, End, CollisionChannel, Params, FCollisionResponseParams::DefaultResponseParam, FCollisionObjectQueryParams::DefaultObjectQueryParam);

	Hit = HitResult.bBlockingHit;
	HitLoc = HitResult.Location;
	HitNormal = HitResult.Normal;
}

void UPassiveRagdollComponent::Server_RagdollStart_Implementation()
{
	Multicast_RagdollStart();
}
void UPassiveRagdollComponent::Multicast_RagdollStart_Implementation()
{	
	Mesh->SetCollisionObjectType(ECC_PhysicsBody);	
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);	
	Mesh->SetAllBodiesBelowSimulatePhysics(Pelvis, true, true);
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);	
	CharMov->SetMovementMode(MOVE_None);

	if (!bGetUpSelf)
	{
		Actor->SetReplicateMovement(false);
	}

	bInRagdoll = true;
}

void UPassiveRagdollComponent::Server_RagdollUpdate_Implementation(FVector InActorLoc, FRotator InActorRot, FVector InMeshLoc, FQuat InMeshQuat)
{
	Multicast_RagdollUpdate(InActorLoc, InActorRot, InMeshLoc, InMeshQuat);
}
void UPassiveRagdollComponent::Multicast_RagdollUpdate_Implementation(FVector InActorLoc, FRotator InActorRot, FVector InMeshLoc, FQuat InMeshQuat)
{
	Actor->SetActorLocation(InActorLoc);
	Actor->SetActorRotation(InActorRot);

	if (!Char->IsLocallyControlled()) {
		FVector Init_MeshLoc = Mesh->GetComponentLocation();
		FRotator Init_MeshRot = Mesh->GetComponentRotation();
		FQuat Init_MeshQuat(Init_MeshRot);

		FQuat MeshQuat = FQuat::Slerp(Init_MeshQuat, InMeshQuat, MeshRotAlpha);
		FRotator MeshRot = MeshQuat.Rotator();

		Mesh->SetWorldLocation(Init_MeshLoc + MeshLocAlpha * (InMeshLoc - Init_MeshLoc), false, nullptr, ETeleportType::TeleportPhysics);
		Mesh->SetWorldRotation(MeshRot, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void UPassiveRagdollComponent::Server_CanEnterRagdollEnd_Implementation()
{
	Multicast_CanEnterRagdollEnd();
}
void UPassiveRagdollComponent::Multicast_CanEnterRagdollEnd_Implementation()
{
	bRagdollEnd = true;
}

void UPassiveRagdollComponent::RagdollEnd()
{
	if (bRagdollEnd) {
		Server_RagdollEnd();
	}
}
void UPassiveRagdollComponent::Server_RagdollEnd_Implementation()
{
	Multicast_RagdollEnd();
}
void UPassiveRagdollComponent::Multicast_RagdollEnd_Implementation()
{
	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	float CapsuleHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();
	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);
	FVector ActorLocation = Actor->GetActorLocation();
	FRotator ActorRotation = Actor->GetActorRotation();
	FVector ToBeMashLocation = ActorLocation + FVector(0.0f, 0.0f, -CapsuleHalfHeight);
	FRotator ToBeMeshRotation = ActorRotation + FRotator(0.0f, -90.0f, 0.0f);

	RagdollEnd_RootTM = Mesh->GetSocketTransform(Root);
	FPassiveRagdollStruct Struct{ true, RagdollEnd_RootTM, PoseSnapshot };

	bInRagdoll = false;
	bRagdollEnd = false;
	bCanGetUp = true;

	CharMov->SetMovementMode(MOVE_Walking);
	AnimInstance->SavePoseSnapshot(PoseSnapshot);
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_Pawn);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetAllBodiesSimulatePhysics(false);	
	Mesh->SetWorldLocationAndRotation(ToBeMashLocation, ToBeMeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	Mesh->AttachToComponent(Capsule, AttachmentRules);	

	if (!bGetUpSelf)
	{
		Actor->SetReplicateMovement(true);
	}

	RagdollEndDelegate.Broadcast(Struct, AnimInstance);
}

void UPassiveRagdollComponent::GetUp()
{
	if (bCanGetUp) {	
		UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
		FRotator PelvisRot = Mesh->GetSocketRotation(Pelvis);
		UAnimMontage* GetUpMontage = PelvisRot.Roll > 0.0f ? Montage_F : Montage_B;

		Server_GetUp(GetUpMontage);
	}
}
void UPassiveRagdollComponent::Server_GetUp_Implementation(UAnimMontage* GetUpMontage)
{
	Multicast_GetUp(GetUpMontage);
}
void UPassiveRagdollComponent::Multicast_GetUp_Implementation(UAnimMontage* GetUpMontage)
{
	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	AnimInstance->Montage_Play(GetUpMontage);

	FPassiveRagdollStruct Struct{ false, RagdollEnd_RootTM, PoseSnapshot };

	bRagdollEnd = false;
	bCanGetUp = false;

	GetUpDelegate.Broadcast(Struct, AnimInstance);
}