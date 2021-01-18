// Copyright Stephen Maloney, 2020

#pragma once

#include "CoreMinimal.h"

#include "Core/PoolInterface.h"
#include "GameFramework/Actor.h"
#include "HitEffectActor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class TARGETSHOOT_API AHitEffectActor : public AActor, public IPoolInterface
{
	GENERATED_BODY()
	
public:	

	AHitEffectActor();

	virtual void PoolInterface_DestroyOrReturnToPool() override;

	virtual void PoolInterface_Initialize() override;

	virtual void PoolInterface_OnReturnToPool() override;

	virtual void PoolInterface_OnPullFromPool(FTransform Transform) override;
	
protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSystemFinished(UNiagaraComponent* PSystem);

	UPROPERTY(Transient)
	ATargetShootGameModeBase* GameMode;
	
	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* NiagaraComponent;
	
};
