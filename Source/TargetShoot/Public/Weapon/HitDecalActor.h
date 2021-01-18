// Copyright Stephen Maloney, 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/PoolInterface.h"
#include "HitDecalActor.generated.h"

UCLASS()
class TARGETSHOOT_API AHitDecalActor : public AActor, public IPoolInterface
{
	GENERATED_BODY()
	
public:	
	
	AHitDecalActor();

	virtual void PoolInterface_Initialize() override;

	virtual void PoolInterface_OnReturnToPool() override;

	virtual void PoolInterface_DestroyOrReturnToPool() override;

	virtual void PoolInterface_OnPullFromPool(FTransform Transform) override;
	
protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	UDecalComponent* DecalComponent;

	UPROPERTY(Transient)
	ATargetShootGameModeBase* GameMode;
	
};
