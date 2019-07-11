// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h" 
#include "QuadBallGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class QUADBALL_API UQuadBallGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMesh* playerAMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMesh* playerBMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int gameMode = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int debugMode = 0;

};
