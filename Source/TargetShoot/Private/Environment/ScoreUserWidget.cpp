// Copyright Stephen Maloney, 2020

#include "Environment/ScoreUserWidget.h"

#include "Components/TextBlock.h"

void UScoreUserWidget::SetAverageText(const FString Text) const
{
	AverageText->SetText(FText::AsCultureInvariant(*Text));
}

void UScoreUserWidget::SetScoreText(const FString Text) const
{
	ScoreText->SetText(FText::AsCultureInvariant(*Text));
}

void UScoreUserWidget::SetShotsFiredText(const FString Text) const
{
	ShotsFiredText->SetText(FText::AsCultureInvariant(*Text));
}
