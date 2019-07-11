// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuadBallUI.generated.h"

/**
 * 
 */
UCLASS()
class QUADBALL_API UQuadBallUI : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetAimerLocation(float x, float y);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowPause();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HidePause();
};
