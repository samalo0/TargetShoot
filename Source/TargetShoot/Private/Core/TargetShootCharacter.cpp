// Copyright Stephen Maloney, 2020

#include "Core/TargetShootCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Components/InputComponent.h"
#include "MotionControllerComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapon/WeaponActor.h"

ATargetShootCharacter::ATargetShootCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetMesh()->SetCollisionProfileName("NoCollision");
	GetMesh()->SetGenerateOverlapEvents(false);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	
	VROffsetComponent = CreateDefaultSubobject<USceneComponent>(TEXT("VROffsetComponent"));
	VROffsetComponent->SetupAttachment(GetRootComponent());
	VROffsetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(VROffsetComponent);

	LeftMotionControllerComponent = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftMotionControllerComponent"));
	LeftMotionControllerComponent->SetupAttachment(VROffsetComponent);
	LeftMotionControllerComponent->SetGenerateOverlapEvents(false);
	LeftMotionControllerComponent->SetCollisionProfileName("NoCollision");

	RightMotionControllerComponent = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightMotionControllerComponent"));
	RightMotionControllerComponent->SetupAttachment(VROffsetComponent);
	RightMotionControllerComponent->SetGenerateOverlapEvents(false);
	RightMotionControllerComponent->SetCollisionProfileName("NoCollision");
	RightMotionControllerComponent->MotionSource = "Right";
}

void ATargetShootCharacter::BeginPlay()
{
	Super::BeginPlay();

	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	UWorld* World = GetWorld();

	if(!IsValid(World))
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;
	SpawnParameters.ObjectFlags |= RF_Transient;
	
	if(IsValid(LeftWeaponActorClass))
	{
		LeftWeaponActor = World->SpawnActor<AWeaponActor>(LeftWeaponActorClass, SpawnParameters);
		if(IsValid(LeftWeaponActor))
		{
			LeftWeaponActor->AttachToComponent(LeftMotionControllerComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}

	if(IsValid(RightWeaponActorClass))
	{
		RightWeaponActor = World->SpawnActor<AWeaponActor>(RightWeaponActorClass, SpawnParameters);
		if(IsValid(RightWeaponActor))
		{
			RightWeaponActor->AttachToComponent(RightMotionControllerComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}
}

void ATargetShootCharacter::MoveForward(const float AxisValue)
{
	AddMovementInput(CameraComponent->GetForwardVector(), AxisValue);
}

void ATargetShootCharacter::MoveRight(const float AxisValue)
{
	AddMovementInput(CameraComponent->GetRightVector(), AxisValue);
}

void ATargetShootCharacter::OnLeftShoot()
{
	if(IsValid(LeftWeaponActor))
	{
		LeftWeaponActor->Fire();
	}
}

void ATargetShootCharacter::OnRightShoot()
{
	if(IsValid(RightWeaponActor))
	{
		RightWeaponActor->Fire();
	}
}

void ATargetShootCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("LeftShoot", EInputEvent::IE_Pressed, this, &ATargetShootCharacter::OnLeftShoot);
	PlayerInputComponent->BindAction("RightShoot", EInputEvent::IE_Pressed, this, &ATargetShootCharacter::OnRightShoot);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATargetShootCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATargetShootCharacter::MoveRight);
}


