// Written by Berkay TUna, October 2020

#include "MountCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RidingSystemComponent.h"
#include "Engine/Engine.h"

AMountCharacter::AMountCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	MountLoc_Left = CreateDefaultSubobject<USceneComponent>(TEXT("MountLoc_Left"));
	MountLoc_Left->SetupAttachment(GetMesh());
	MountLoc_Right = CreateDefaultSubobject<USceneComponent>(TEXT("MountLoc_Right"));
	MountLoc_Right->SetupAttachment(GetMesh());

	MountArea_Left = CreateDefaultSubobject<UBoxComponent>(TEXT("MountArea_Left"));
	MountArea_Left->SetupAttachment(GetMesh());
	MountArea_Right = CreateDefaultSubobject<UBoxComponent>(TEXT("MountArea_Right"));
	MountArea_Right->SetupAttachment(GetMesh());
}

void AMountCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMountCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMountCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMountCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMountCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMountCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMountCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMountCharacter::OnResetVR);

	//PlayerInputComponent->BindAction("Dismount", IE_Pressed, this, &AMountCharacter::Dismount);
}

void AMountCharacter::OnResetVR()
{
	//UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMountCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AMountCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AMountCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMountCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMountCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMountCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMountCharacter::BeginPlay()
{
	Super::BeginPlay();

	Mesh = GetMesh();
	Capsule = GetCapsuleComponent();
}

void AMountCharacter::GetMountScene(USceneComponent*& MountScene_Left, USceneComponent*& MountScene_Right)
{
	MountScene_Left = MountLoc_Left;
	MountScene_Right = MountLoc_Right;
}

void AMountCharacter::Mount(ACharacter* InRiderChar, FTransform PlayerCamManagerTM)
{
	if (InRiderChar)
	{
		RiderChar = InRiderChar;
		RiderActor = RiderChar;

		Mesh->IgnoreActorWhenMoving(RiderActor, true);
		Capsule->IgnoreActorWhenMoving(RiderActor, true);
	
		if (bCameraTransforms)
		{
			//APlayerCameraManager* PlayerCamManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
			//FTransform PlayerCamManagerTM = PlayerCamManager->GetActorTransform();
			FollowCamera->SetWorldTransform(PlayerCamManagerTM, false, nullptr, ETeleportType::TeleportPhysics);

			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			UKismetSystemLibrary::MoveComponentTo(FollowCamera, FVector::ZeroVector, FRotator::ZeroRotator, false, false, CameraTransformDuration, false,
				EMoveComponentAction::Type::Move, LatentInfo);
		}

		bIsRiding = true;
	}
}



void AMountCharacter::Dismount(URidingSystemComponent* RidingSystemComp, UCameraComponent* RiderCamera, UAnimMontage* DismountMontage, EDismountEnum DismountFrom,
	float DismountTurnBeginTime, float DismountTurnDuration, float GroundedTime)
{
	if (bIsRiding && RidingSystemComp)
	{
		USceneComponent* DismountScene;
		if (DismountFrom == EDismountEnum::Left)
		{
			DismountScene = MountLoc_Left;
		}
		else
		{
			DismountScene = MountLoc_Right;
		}

		APlayerController* PossessingPlayerController = UGameplayStatics::GetPlayerController(this, 0);

		Server_Dismount(PossessingPlayerController, DismountScene, RidingSystemComp, RiderCamera, DismountMontage, DismountTurnBeginTime, DismountTurnDuration, GroundedTime);
	}
}

void AMountCharacter::Server_Dismount_Implementation(APlayerController* PossessingPlayerController, USceneComponent* InDismountScene, URidingSystemComponent* RidingSystemComp, 
	UCameraComponent* RiderCamera, UAnimMontage* DismountMontage, float DismountTurnBeginTime, float DismountTurnDuration, float GroundedTime)
{
	FTransform RiderActorTM = RiderActor->GetActorTransform();

	Multi_Dismount(PossessingPlayerController, RiderActorTM, InDismountScene, RidingSystemComp, RiderCamera, DismountMontage, DismountTurnBeginTime, DismountTurnDuration, GroundedTime);
}

void AMountCharacter::Multi_Dismount_Implementation(APlayerController* PossessingPlayerController, FTransform RiderActorTM, USceneComponent* InDismountScene, URidingSystemComponent* RidingSystemComp, 
	UCameraComponent* RiderCamera, UAnimMontage* DismountMontage, float DismountTurnBeginTime, 
	float DismountTurnDuration, float GroundedTime)
{
	bIsRiding = false;

	Mesh->IgnoreActorWhenMoving(RiderActor, false);
	Capsule->IgnoreActorWhenMoving(RiderActor, false);

	USkeletalMeshComponent* RiderMesh = RiderChar->GetMesh();
	UCapsuleComponent* RiderCapsule = RiderChar->GetCapsuleComponent();

	RiderMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	RiderCapsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	RidingSystemComp->Dismount(PossessingPlayerController, RiderActorTM, InDismountScene, DismountMontage, RiderCamera, DismountTurnBeginTime, DismountTurnDuration, GroundedTime);
}