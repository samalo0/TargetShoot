// Copyright Stephen Maloney, 2020

#include "Environment/TargetActor.h"

#include "Weapon/ProjectileActor.h"
#include "Components/StaticMeshComponent.h"
#include "Core/TargetShootGameModeBase.h"
#include "Engine.h"
#include "Kismet/KismetRenderingLibrary.h"

ATargetActor::ATargetActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	HandleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandleComponent"));
	SetRootComponent(HandleComponent);
	HandleComponent->SetMobility(EComponentMobility::Static);
	HandleComponent->SetGenerateOverlapEvents(false);
	HandleComponent->SetCollisionProfileName("BlockAll");

	TargetComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetComponent"));
	TargetComponent->SetupAttachment(GetRootComponent());
	TargetComponent->SetMobility(EComponentMobility::Static);
	TargetComponent->SetGenerateOverlapEvents(false);
	TargetComponent->SetCollisionProfileName("BlockAll");
}

void ATargetActor::BeginPlay()
{
	Super::BeginPlay();

	TargetComponent->OnComponentHit.AddDynamic(this, &ATargetActor::OnTargetHit);

	UWorld* World = GetWorld();
	if(IsValid(World))
	{
		GameMode = Cast<ATargetShootGameModeBase>(World->GetAuthGameMode());
	}

	if(IsValid(RenderTargetMaterial))
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(RenderTargetMaterial, this);
	}

	if(IsValid(RenderTarget))
	{
		UKismetRenderingLibrary::ClearRenderTarget2D(this, RenderTarget);
	}
}

void ATargetActor::OnTargetHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	AProjectileActor* ProjectileActor = Cast<AProjectileActor>(OtherActor);
	if(IsValid(ProjectileActor))
	{
		const FVector2D WorldHitLocation = FVector2D(Hit.ImpactPoint.X, Hit.ImpactPoint.Z);
		const FVector2D Center2D = FVector2D(CenterLocation.X + GetActorLocation().X, CenterLocation.Z + GetActorLocation().Z);
		const float Distance = FVector2D::Distance(WorldHitLocation, Center2D);
		const float Score = 10.0f * (1.0f - Distance / 24.0f);

		if(IsValid(GameMode))
		{
			GameMode->AddScore(FMath::RoundToInt(Score));
		}

		UWorld* World = GetWorld();
		if(IsValid(RenderTarget) && IsValid(DynamicMaterial) && IsValid(World))
		{
			FHitResult OutHit;
			FCollisionQueryParams Params;
			Params.bTraceComplex = true;
			if(TargetComponent->LineTraceComponent(OutHit, Hit.TraceStart, Hit.TraceEnd, Params))
			{
				FVector2D UV;
				UGameplayStatics::FindCollisionUV(OutHit, 0, UV);

				DynamicMaterial->SetVectorParameterValue("Location", FLinearColor(UV.X, UV.Y, 0.0f, 0.0f));
				UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, RenderTarget, DynamicMaterial);
			}
		}
	}
}
