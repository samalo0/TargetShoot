// Copyright Stephen Maloney, 2020

#include "Core/TeleportTargetShootPawn.h"


#include "HeadMountedDisplayFunctionLibrary.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "MotionControllerComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "TimerManager.h"

ATeleportTargetShootPawn::ATeleportTargetShootPawn()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    TeleportCylinderComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportCylinderComponent"));
    TeleportCylinderComponent->SetupAttachment(GetRootComponent());
    TeleportCylinderComponent->SetCastShadow(false);
    TeleportCylinderComponent->SetCollisionProfileName("NoCollision");
    TeleportCylinderComponent->SetGenerateOverlapEvents(false);

    ArcArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArcArrowComponent"));
    ArcArrowComponent->ArrowSize = 0.2f;
    ArcArrowComponent->SetupAttachment(RightMotionControllerComponent);

    ArcSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("ArcSplineComponent"));
    ArcSplineComponent->SetupAttachment(RightMotionControllerComponent);
    ArcSplineComponent->SetCastShadow(false);
    ArcSplineComponent->SetCollisionProfileName("NoCollision");
    ArcSplineComponent->SetGenerateOverlapEvents(false);

    ArcEndComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArcEndComponent"));
    ArcEndComponent->SetupAttachment(GetRootComponent());
    ArcEndComponent->SetCastShadow(false);
    ArcEndComponent->SetVisibility(false);
    ArcEndComponent->SetCollisionProfileName("NoCollision");
    ArcEndComponent->SetGenerateOverlapEvents(false);

    BeaconDirectionComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeaconDirectionComponent"));
    BeaconDirectionComponent->SetupAttachment(TeleportCylinderComponent);
    BeaconDirectionComponent->SetCastShadow(false);
    BeaconDirectionComponent->SetCollisionProfileName("NoCollision");
    BeaconDirectionComponent->SetGenerateOverlapEvents(false);
}

void ATeleportTargetShootPawn::BeginPlay()
{
    Super::BeginPlay();

    TeleportCylinderComponent->SetVisibility(false, true);
}

void ATeleportTargetShootPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MotionControllerThumbLeft_X", this, &ATeleportTargetShootPawn::SetMotionControllerThumbLeftX);
    PlayerInputComponent->BindAxis("MotionControllerThumbLeft_Y", this, &ATeleportTargetShootPawn::SetMotionControllerThumbLeftY);
    PlayerInputComponent->BindAxis("MotionControllerThumbRight_X", this, &ATeleportTargetShootPawn::SetMotionControllerThumbRightX);
    PlayerInputComponent->BindAxis("MotionControllerThumbRight_Y", this, &ATeleportTargetShootPawn::SetMotionControllerThumbRightY);
    
    PlayerInputComponent->BindAction("TeleportLeft", IE_Pressed, this, &ATeleportTargetShootPawn::PressTeleportLeft);
    PlayerInputComponent->BindAction("TeleportLeft", IE_Released, this, &ATeleportTargetShootPawn::ReleaseTeleportLeft);
    PlayerInputComponent->BindAction("TeleportRight", IE_Pressed, this, &ATeleportTargetShootPawn::PressTeleportRight);
    PlayerInputComponent->BindAction("TeleportRight", IE_Released, this, &ATeleportTargetShootPawn::ReleaseTeleportRight);
}

void ATeleportTargetShootPawn::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if(bIsTeleporterActive)
    {
        ClearArc();

        TArray<FVector> TracePoints;
        FVector TeleportLocation;
        FVector TraceLocation;
        bFoundValidTeleportDestination = TraceTeleportDestination(TracePoints, TeleportLocation, TraceLocation);
        TeleportCylinderComponent->SetVisibility(bFoundValidTeleportDestination, true);

        if(bFoundValidTeleportDestination)
        {
            TeleportCylinderComponent->SetWorldLocation(TeleportLocation);
        }

        UpdateArcSpline(bFoundValidTeleportDestination, TracePoints);
        UpdateArcEndPoint(bFoundValidTeleportDestination, TraceLocation);
    }
}

#pragma region Control

void ATeleportTargetShootPawn::ClearArc()
{
    for(USplineMeshComponent* SplineMesh : SplineMeshes)
    {
        SplineMesh->DestroyComponent();
    }
    SplineMeshes.Empty();

    ArcSplineComponent->ClearSplinePoints();
}

void ATeleportTargetShootPawn::ExecuteTeleportation()
{
    if(bIsCurrentlyTeleporting)
    {
        return;
    }

    SetActorTickEnabled(false);
    
    if(bFoundValidTeleportDestination)
    {
        UWorld* World = GetWorld();
        if(!IsValid(World))
        {
            return;
        }

        UGameplayStatics::GetPlayerCameraManager(World, 0)->StartCameraFade(0.0f, 1.0f, FadeOutDuration, FColor::Black, false, true);

        FTimerHandle TimerHandle;
        World->GetTimerManager().SetTimer(TimerHandle, this, &ATeleportTargetShootPawn::ExecuteTeleportationAfterFade, FadeOutDuration);
    }
    else
    {
        ClearArc();
        bIsTeleporterActive = false;
    }
}

void ATeleportTargetShootPawn::ExecuteTeleportationAfterFade()
{
    ClearArc();
    TeleportCylinderComponent->SetVisibility(false, true);
    ArcEndComponent->SetVisibility(false);
    bIsCurrentlyTeleporting = false;
    bIsTeleporterActive = false;

    FVector OutLocation;
    FRotator OutRotation;
    GetTeleportDestination(OutLocation, OutRotation);

    TeleportTo(OutLocation, OutRotation);

    UWorld* World = GetWorld();
    if(IsValid(World))
    {
        UGameplayStatics::GetPlayerCameraManager(World, 0)->StartCameraFade(1.0f, 0.0f, FadeInDuration, FColor::Black, false, false);
    }
}

FRotator ATeleportTargetShootPawn::GetRotationFromInput(const FVector2D Values)
{
    if(FMath::Abs(Values.Y) + FMath::Abs(Values.X) >= ThumbDeadZone)
    {
        const FVector NormalVector = FVector(Values.Y, Values.X, 0.0f).GetSafeNormal();
        const FRotator Rotation = FRotator(0.0f, GetActorRotation().Yaw, 0.0f);

        const FVector RotatedVector = Rotation.RotateVector(NormalVector);

        return RotatedVector.ToOrientationRotator();
    }

    return GetActorRotation();
}

void ATeleportTargetShootPawn::GetTeleportDestination(FVector& OutLocation, FRotator& OutRotation) const
{
    FRotator DeviceRotation;
    FVector DevicePosition;
    UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);

    const FVector DevicePositionNoZ = FVector(DevicePosition.X, DevicePosition.Y, 0.0f);

    OutLocation = TeleportCylinderComponent->GetComponentLocation() - TeleportRotation.RotateVector(DevicePositionNoZ);
    OutRotation = TeleportRotation;
}

void ATeleportTargetShootPawn::PressTeleportLeft()
{
    bIsTeleporterActive = true;
    InitialControllerRotation = LeftMotionControllerComponent->GetComponentRotation();

    ArcSplineComponent->AttachToComponent(LeftMotionControllerComponent, FAttachmentTransformRules::KeepRelativeTransform);
    ArcArrowComponent->AttachToComponent(LeftMotionControllerComponent, FAttachmentTransformRules::KeepRelativeTransform);

    SetActorTickEnabled(true);
}

void ATeleportTargetShootPawn::PressTeleportRight()
{
    bIsTeleporterActive = true;
    InitialControllerRotation = RightMotionControllerComponent->GetComponentRotation();

    ArcSplineComponent->AttachToComponent(RightMotionControllerComponent, FAttachmentTransformRules::KeepRelativeTransform);
    ArcArrowComponent->AttachToComponent(RightMotionControllerComponent, FAttachmentTransformRules::KeepRelativeTransform);

    SetActorTickEnabled(true);
}

void ATeleportTargetShootPawn::ReleaseTeleportLeft()
{
    ExecuteTeleportation();
}

void ATeleportTargetShootPawn::ReleaseTeleportRight()
{
    ExecuteTeleportation();
}

void ATeleportTargetShootPawn::SetMotionControllerThumbLeftX(const float AxisValue)
{
    if(AxisValue != MotionControllerThumbLeft.X)
    {
        MotionControllerThumbLeft.X = AxisValue;
        TeleportRotation = GetRotationFromInput(MotionControllerThumbLeft);
    }
}

void ATeleportTargetShootPawn::SetMotionControllerThumbLeftY(const float AxisValue)
{
    if(AxisValue != MotionControllerThumbLeft.Y)
    {
        MotionControllerThumbLeft.Y = AxisValue;
        TeleportRotation = GetRotationFromInput(MotionControllerThumbLeft);
    }
}

void ATeleportTargetShootPawn::SetMotionControllerThumbRightX(const float AxisValue)
{
    if(AxisValue != MotionControllerThumbRight.X)
    {
        MotionControllerThumbRight.X = AxisValue;
        TeleportRotation = GetRotationFromInput(MotionControllerThumbRight);
    }
}

void ATeleportTargetShootPawn::SetMotionControllerThumbRightY(const float AxisValue)
{
    if(AxisValue != MotionControllerThumbRight.Y)
    {
        MotionControllerThumbRight.Y = AxisValue;
        TeleportRotation = GetRotationFromInput(MotionControllerThumbRight);
    }
}

bool ATeleportTargetShootPawn::TraceTeleportDestination(TArray<FVector>& TracePoints, FVector& TeleportLocation,
    FVector& TraceLocation) const
{
    FPredictProjectilePathParams PredictParams(0.0f, ArcArrowComponent->GetComponentLocation(), ArcArrowComponent->GetForwardVector() * TeleportLaunchVelocity, 2.0f, UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
    FPredictProjectilePathResult PredictResult;
    const bool PredictProjectilePathResult = UGameplayStatics::PredictProjectilePath(this, PredictParams, PredictResult);

    if(!PredictProjectilePathResult)
    {
        return false;
    }

    TraceLocation = PredictResult.HitResult.Location;
    TracePoints.Empty(PredictResult.PathData.Num());
    for(const FPredictProjectilePathPointData& PathPoint : PredictResult.PathData)
    {
        TracePoints.Add(PathPoint.Location);
    }

    TeleportLocation = PredictResult.HitResult.Location;

    return true;
}

void ATeleportTargetShootPawn::UpdateArcEndPoint(const bool bFoundValidLocation, const FVector NewLocation) const
{
    ArcEndComponent->SetVisibility(bFoundValidLocation);
    if(!bFoundValidLocation)
    {
        return;
    }

    ArcEndComponent->SetWorldLocation(NewLocation);

    FRotator DeviceRotation;
    FVector DevicePosition;
    UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);

    const FRotator NewBeaconRotation = FRotator(TeleportRotation.Quaternion() * FRotator(0.0f, DeviceRotation.Yaw, 0.0f).Quaternion());

    BeaconDirectionComponent->SetWorldRotation(NewBeaconRotation);
}

void ATeleportTargetShootPawn::UpdateArcSpline(const bool bFoundValidLocation, TArray<FVector> SplinePoints)
{
    if(!bFoundValidLocation)
    {
        SplinePoints.Empty(2);
        SplinePoints.Add(ArcArrowComponent->GetComponentLocation());
        SplinePoints.Add(ArcArrowComponent->GetComponentLocation() + ArcArrowComponent->GetForwardVector() * 20.0f);
    }

    for(FVector& SplinePoint : SplinePoints)
    {
        ArcSplineComponent->AddSplinePoint(SplinePoint, ESplineCoordinateSpace::Local, true);
    }

    ArcSplineComponent->SetSplinePointType(ArcSplineComponent->GetNumberOfSplinePoints() - 1, ESplinePointType::CurveClamped, true);

    for(auto Index = 0; Index < ArcSplineComponent->GetNumberOfSplinePoints() - 2; Index++)
    {
        USplineMeshComponent* Component = NewObject<USplineMeshComponent>(this);
        Component->RegisterComponent();

        Component->SetMobility(EComponentMobility::Movable);
        Component->SetCastShadow(false);
        Component->SetRelativeLocation(FVector::ZeroVector);
        Component->SetStaticMesh(TeleportBeamMesh);

        SplineMeshes.Add(Component);

        Component->SetStartAndEnd(SplinePoints[Index], ArcSplineComponent->GetTangentAtSplinePoint(Index, ESplineCoordinateSpace::Local), SplinePoints[Index + 1],
            ArcSplineComponent->GetTangentAtSplinePoint(Index + 1, ESplineCoordinateSpace::Local));
    }
}

#pragma endregion
