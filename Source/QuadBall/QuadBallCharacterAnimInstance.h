// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "QuadBallCharacter.h"
#include "QuadBallCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class QUADBALL_API UQuadBallCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	
public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Head();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void RightKick();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void LeftKick();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Knee();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandstandOn();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandstandOff();
};
