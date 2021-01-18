// Copyright Stephen Maloney, 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

class UAnimSequence;
class ATargetShootGameModeBase;
class AProjectileActor;
class UNiagaraComponent;
class UArrowComponent;

UCLASS(HideCategories = (Tick, Rendering, Input, Actor, LOD))
class TARGETSHOOT_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	

	AWeaponActor();

	UFUNCTION(CallInEditor, Category = "Test")
	void Fire() const;

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif
	
protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	UAnimSequence* FireAnimation;
	
	UPROPERTY(EditDefaultsOnly)
	USoundBase* FireSound;
	
	UPROPERTY(Transient)
	ATargetShootGameModeBase* GameMode;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* NiagaraComponent;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectileActor> ProjectileClass;

#if WITH_EDITORONLY_DATA
	
	UPROPERTY(VisibleAnywhere)
	UArrowComponent* ProjectileSpawnArrowComponent;

#endif
	
	UPROPERTY(VisibleDefaultsOnly)
	FTransform ProjectileSpawnRelativeTransform;
	
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMeshComponent;
	
};

