// Copyright Stephen Maloney, 2020

#include "Environment/LightRailActor.h"

#include "Components/StaticMeshComponent.h"

ALightRailActor::ALightRailActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);
	StaticMeshComponent->SetMobility(EComponentMobility::Static);
	StaticMeshComponent->SetGenerateOverlapEvents(false);
	StaticMeshComponent->SetCollisionProfileName("BlockAll");

	SpotLightComponent1 = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightComponent1"));
	SpotLightComponent1->SetupAttachment(GetRootComponent());
	SpotLightComponent1->SetMobility(EComponentMobility::Static);
	
	SpotLightComponent2 = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightComponent2"));
	SpotLightComponent2->SetupAttachment(GetRootComponent());
	SpotLightComponent2->SetMobility(EComponentMobility::Static);

	SpotLightComponent3 = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightComponent3"));
	SpotLightComponent3->SetupAttachment(GetRootComponent());
	SpotLightComponent3->SetMobility(EComponentMobility::Static);
}
