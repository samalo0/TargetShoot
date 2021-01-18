// Copyright Stephen Maloney, 2020

#pragma once

#include "CoreMinimal.h"

#include "Components/SpotLightComponent.h"
#include "GameFramework/Actor.h"
#include "LightRailActor.generated.h"

/**
 * Class for a light rail.
 */
UCLASS()
class TARGETSHOOT_API ALightRailActor : public AActor
{

	GENERATED_BODY()
	
public:	

	ALightRailActor();

protected:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere)
	USpotLightComponent* SpotLightComponent1;

	UPROPERTY(VisibleAnywhere)
	USpotLightComponent* SpotLightComponent2;

	UPROPERTY(VisibleAnywhere)
	USpotLightComponent* SpotLightComponent3;
	
};
