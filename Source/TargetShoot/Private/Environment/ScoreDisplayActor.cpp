// Copyright Stephen Maloney, 2020

#include "Environment/ScoreDisplayActor.h"

#include "Components/TextRenderComponent.h"
#include "Core/TargetShootGameModeBase.h"

AScoreDisplayActor::AScoreDisplayActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ScoreTextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScoreTextRenderComponent"));
	SetRootComponent(ScoreTextRenderComponent);

	ShotsFiredTextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ShotsFiredTextRenderComponent"));
	ShotsFiredTextRenderComponent->SetupAttachment(GetRootComponent());

	AverageTextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("AverageTextRenderComponent"));
	AverageTextRenderComponent->SetupAttachment(GetRootComponent());
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
	ScoreTextRenderComponent->SetText(FText::AsCultureInvariant(FString::Printf(TEXT("Score: %d"), Score)));
	ShotsFiredTextRenderComponent->SetText( FText::AsCultureInvariant(FString::Printf(TEXT("Shots Fired: %d"), NumberOfShotsFired)));
	AverageTextRenderComponent->SetText(FText::AsCultureInvariant(FString::Printf(TEXT("Average: %f"), static_cast<float>(Score)/NumberOfShotsFired)));
}
