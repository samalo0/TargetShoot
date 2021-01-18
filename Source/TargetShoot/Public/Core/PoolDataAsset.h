// Copyright Stephen Maloney, 2020

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PoolDataAsset.generated.h"

USTRUCT()
struct FPoolData
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Class;

	UPROPERTY(EditAnywhere)
	int32 Count = 16;

	UPROPERTY(EditAnywhere)
	bool bEnabled = true;

	UPROPERTY(EditAnywhere)
	bool bSpawnMoreWhenDepleted = false;
	
};

/**
 * Class for the pool data asset in editor.
 */
UCLASS()
class TARGETSHOOT_API UPoolDataAsset : public UDataAsset
{

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FPoolData> PooledActors;
	
};
