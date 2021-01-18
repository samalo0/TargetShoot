// Copyright Stephen Maloney, 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TargetShootGameModeBase.generated.h"

class AHitDecalActor;
class UPoolDataAsset;

USTRUCT()
struct FPooledActor
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UClass* ActorClass;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> Active;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> Inactive;

	UPROPERTY(EditAnywhere)
	bool bSpawnMoreWhenDepleted = false;
	
#if WITH_EDITORONLY_DATA

	UPROPERTY(EditAnywhere)
	FName ActiveFolderName = "Active";
	
	UPROPERTY(EditAnywhere)
	FName InactiveFolderName = "Inactive";

#endif
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FScoreUpdateSignature, int32, Score, int32, NumberOfShotsFired);

/**
 * Base class for the game mode. 
 */
UCLASS()
class TARGETSHOOT_API ATargetShootGameModeBase : public AGameModeBase
{

	GENERATED_BODY()

public:

	ATargetShootGameModeBase();
	
protected:

	virtual void BeginPlay() override;

#pragma region Decal

public:

	void RegisterDecal(AHitDecalActor* Decal);

	void UnregisterDecal(AHitDecalActor* Decal);
	
protected:

	FCriticalSection HitDecalActorsCriticalSection;
	
	UPROPERTY(Transient, VisibleInstanceOnly)
	TArray<AHitDecalActor*> HitDecalActors;

	UPROPERTY(EditDefaultsOnly)
	int32 MaximumNumberOfDecals = 20;
	
#pragma endregion
	
#pragma region Pool

public:

	AActor* PullActorFromPool(UClass* Class, FTransform Transform);
	
	void ReturnActorToPool(AActor* Actor);
	
protected:

	void SpawnActorsForPool();
	
	UPROPERTY(Transient, VisibleInstanceOnly)
	TArray<FPooledActor> Pool;

	UPROPERTY(EditDefaultsOnly)
	UPoolDataAsset* PoolDataAsset;
	
#pragma endregion

#pragma region Score

public:

	void AddScore(int32 Score);

	UPROPERTY(BlueprintAssignable)
	FScoreUpdateSignature OnScoreUpdate;
	
protected:

	int32 TotalScore = 0;

	int32 TotalShotsFired = 0;
	
#pragma endregion
	
};
