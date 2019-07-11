// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "QuadBallTrainer.generated.h"

UCLASS()
class QUADBALL_API AQuadBallTrainer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AQuadBallTrainer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool servePending = false;
	bool returned = false;

	int trainerStatus = -1;

	UPROPERTY(EditAnywhere)
	class AQuadBallBall* activeBall;
	UPROPERTY(EditAnywhere)
	class AQuadBallCourt* activeCourt;

	UPROPERTY(EditAnywhere)
	float impactForce = 100;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void SetReturned(bool inVal) { returned = inVal; };

	void ReturnBall();
	
};
