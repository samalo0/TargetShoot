// Copyright Stephen Maloney, 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TargetShootCharacter.generated.h"

class UCameraComponent;
class UMotionControllerComponent;
class USphereComponent;
class AWeaponActor;

UCLASS()
class TARGETSHOOT_API ATargetShootCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ATargetShootCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:

	virtual void BeginPlay() override;

	void MoveForward(float AxisValue);

	void MoveRight(float AxisValue);
	
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
	USceneComponent* VROffsetComponent;

#pragma endregion
	
	
};
