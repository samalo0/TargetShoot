// Copyright Stephen Maloney, 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetActor.generated.h"

class ATargetShootGameModeBase;
class UTextureRenderTarget2D;

UCLASS()
class TARGETSHOOT_API ATargetActor : public AActor
{
	GENERATED_BODY()
	
public:	

	ATargetActor();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTargetHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY(EditDefaultsOnly, meta = (MakeEditWidget = true))
	FVector CenterLocation = FVector::ZeroVector;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* DynamicMaterial;
	
	UPROPERTY(Transient)
	ATargetShootGameModeBase* GameMode;

	UPROPERTY(EditDefaultsOnly)
	UTextureRenderTarget2D* RenderTarget;
	
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* RenderTargetMaterial;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* HandleComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TargetComponent;
	
};
