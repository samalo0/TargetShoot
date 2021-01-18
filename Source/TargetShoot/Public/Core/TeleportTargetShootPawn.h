// Copyright Stephen Maloney, 2020

#pragma once

#include "CoreMinimal.h"
#include "Core/TargetShootPawn.h"
#include "TeleportTargetShootPawn.generated.h"

class UArrowComponent;
class USplineComponent;
class USplineMeshComponent;

/**
 * Subclass of the target shoot pawn, which supports teleportation.
 */
UCLASS()
class TARGETSHOOT_API ATeleportTargetShootPawn : public ATargetShootPawn
{

	GENERATED_BODY()

public:

	ATeleportTargetShootPawn();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;
	
protected:

	virtual void BeginPlay() override;

#pragma region Control

protected:

	void ClearArc();
	
	void ExecuteTeleportation();

	void ExecuteTeleportationAfterFade();
	
	FRotator GetRotationFromInput(FVector2D Values);

	void GetTeleportDestination(FVector& OutLocation, FRotator& OutRotation) const;

	void PressTeleportLeft();

	void PressTeleportRight();
	
	void ReleaseTeleportLeft();

	void ReleaseTeleportRight();
	
	void SetMotionControllerThumbLeftX(float AxisValue);

	void SetMotionControllerThumbLeftY(float AxisValue);

	void SetMotionControllerThumbRightX(float AxisValue);

	void SetMotionControllerThumbRightY(float AxisValue);

	bool TraceTeleportDestination(TArray<FVector>& TracePoints, FVector& TeleportLocation, FVector& TraceLocation) const;

	void UpdateArcEndPoint(bool bFoundValidLocation, FVector NewLocation) const;

	void UpdateArcSpline(bool bFoundValidLocation, TArray<FVector> SplinePoints);
	
	bool bFoundValidTeleportDestination = false;
	
	float FadeInDuration = 0.3f;
	
	float FadeOutDuration = 0.2f;
	
	bool bIsCurrentlyTeleporting = false;
	
	bool bIsTeleporterActive = false;

	FRotator InitialControllerRotation = FRotator::ZeroRotator;

	FVector2D MotionControllerThumbLeft = FVector2D::ZeroVector;

	FVector2D MotionControllerThumbRight = FVector2D::ZeroVector;

	UPROPERTY(Transient)
	TArray<USplineMeshComponent*> SplineMeshes;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* TeleportBeamMesh;
	
	float TeleportLaunchVelocity = 900.0f;
	
	FRotator TeleportRotation = FRotator::ZeroRotator;

	float ThumbDeadZone = 0.3f;
	
#pragma endregion
	
#pragma region Components

protected:

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* ArcArrowComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ArcEndComponent;

	UPROPERTY(VisibleAnywhere)
	USplineComponent* ArcSplineComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BeaconDirectionComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TeleportCylinderComponent;
		
#pragma endregion
	
};
