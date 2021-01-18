// Copyright Stephen Maloney, 2020

#include "Core/TargetShootPawn.h"

#include "Camera/CameraComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Components/InputComponent.h"
#include "MotionControllerComponent.h"
#include "Components/SphereComponent.h"
#include "Weapon/WeaponActor.h"

ATargetShootPawn::ATargetShootPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);
	SphereComponent->SetCollisionProfileName("Pawn");
	SphereComponent->SetGenerateOverlapEvents(false);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetRootComponent());

	LeftMotionControllerComponent = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftMotionControllerComponent"));
	LeftMotionControllerComponent->SetupAttachment(GetRootComponent());
	LeftMotionControllerComponent->SetGenerateOverlapEvents(false);
	LeftMotionControllerComponent->SetCollisionProfileName("NoCollision");

	RightMotionControllerComponent = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightMotionControllerComponent"));
	RightMotionControllerComponent->SetupAttachment(GetRootComponent());
	RightMotionControllerComponent->SetGenerateOverlapEvents(false);
	RightMotionControllerComponent->SetCollisionProfileName("NoCollision");
	RightMotionControllerComponent->MotionSource = "Right";
}

void ATargetShootPawn::BeginPlay()
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

void ATargetShootPawn::OnLeftShoot()
{
	if(IsValid(LeftWeaponActor))
	{
		LeftWeaponActor->Fire();
	}
}

void ATargetShootPawn::OnRightShoot()
{
	if(IsValid(RightWeaponActor))
	{
		RightWeaponActor->Fire();
	}
}

void ATargetShootPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("LeftShoot", EInputEvent::IE_Pressed, this, &ATargetShootPawn::OnLeftShoot);
	PlayerInputComponent->BindAction("RightShoot", EInputEvent::IE_Pressed, this, &ATargetShootPawn::OnRightShoot);
}

