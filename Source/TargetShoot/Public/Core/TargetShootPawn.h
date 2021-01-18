// Copyright Stephen Maloney, 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TargetShootPawn.generated.h"

class UCameraComponent;
class UMotionControllerComponent;
class USphereComponent;
class AWeaponActor;

UCLASS()
class TARGETSHOOT_API ATargetShootPawn : public APawn
{
	GENERATED_BODY()

public:

	ATargetShootPawn();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	virtual void BeginPlay() override;

	void OnLeftShoot();

	void OnRightShoot();

	UPROPERTY(Transient, VisibleInstanceOnly)
	AWeaponActor* LeftWeaponActor;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeaponActor> LeftWeaponActorClass;

	UPROPERTY(Transient, VisibleInstanceOnly)
	AWeaponActor* RightWeaponActor;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeaponActor> RightWeaponActorClass;
	
	
#pragma region Components
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere)
	UMotionControllerComponent* LeftMotionControllerComponent;

	UPROPERTY(VisibleAnywhere)
	UMotionControllerComponent* RightMotionControllerComponent;
	
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

#pragma endregion
	
};

