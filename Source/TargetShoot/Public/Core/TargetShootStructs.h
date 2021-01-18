// Copyright Stephen Maloney, 2020

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TargetShootStructs.generated.h"

USTRUCT(BlueprintType)
struct FMaterialEffectRow : public FTableRowBase
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorToSpawnOnHit;
	
	UPROPERTY(EditAnywhere)
	USoundBase* Sound;
	
};
