// Copyright Stephen Maloney, 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScoreDisplayActor.generated.h"

class UTextRenderComponent;

UCLASS()
class TARGETSHOOT_API AScoreDisplayActor : public AActor
{

	GENERATED_BODY()
	
public:	

	AScoreDisplayActor();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnScoreUpdate(int32 Score, int32 NumberOfShotsFired);

	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* AverageTextRenderComponent;
	
	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* ScoreTextRenderComponent;

	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* ShotsFiredTextRenderComponent;
	
};
