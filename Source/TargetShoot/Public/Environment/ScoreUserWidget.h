// Copyright Stephen Maloney, 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreUserWidget.generated.h"

class UTextBlock;

/**
 * User widget for displaying the score, shots fired, and average.
 */
UCLASS()
class TARGETSHOOT_API UScoreUserWidget : public UUserWidget
{

	GENERATED_BODY()

public:

	void SetAverageText(FString Text) const;

	void SetScoreText(FString Text) const;

	void SetShotsFiredText(FString Text) const;
	
protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AverageText;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ShotsFiredText;

};
