// Copyright Stephen Maloney, 2020

#include "Environment/ScoreDisplayActor.h"

#include "Core/TargetShootGameModeBase.h"
#include "Components/WidgetComponent.h"
#include "Environment/ScoreUserWidget.h"

AScoreDisplayActor::AScoreDisplayActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	SetRootComponent(WidgetComponent);
}

void AScoreDisplayActor::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if(IsValid(World))
	{
		ATargetShootGameModeBase* GameMode = Cast<ATargetShootGameModeBase>(World->GetAuthGameMode());
		if(IsValid(GameMode))
		{
			GameMode->OnScoreUpdate.AddDynamic(this, &AScoreDisplayActor::OnScoreUpdate);
		}
	}
}

void AScoreDisplayActor::OnScoreUpdate(const int32 Score, const int32 NumberOfShotsFired)
{
	UScoreUserWidget* ScoreWidget = Cast<UScoreUserWidget>(WidgetComponent->GetWidget());
	if(IsValid(ScoreWidget))
	{
		ScoreWidget->SetScoreText(FString::Printf(TEXT("Score: %d"), Score));
		ScoreWidget->SetShotsFiredText(FString::Printf(TEXT("Shots Fired: %d"), NumberOfShotsFired));
		ScoreWidget->SetAverageText(FString::Printf(TEXT("Average: %f"), static_cast<float>(Score)/NumberOfShotsFired));	
	}
}
