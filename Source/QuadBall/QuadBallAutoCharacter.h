// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "QuadBallAutoCharacter.generated.h"

UCLASS()
class QUADBALL_API AQuadBallAutoCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AQuadBallAutoCharacter();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class UQuadBallGameInstance* gameInstance;

	class USkeletalMesh* trainerSourceMesh;

	bool servePending = false;
	bool returned = false;

	int trainerStatus = -1;

	UPROPERTY(EditAnywhere)
	class AQuadBallBall* activeBall;
	UPROPERTY(EditAnywhere)
	class AQuadBallCourt* activeCourt;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* kickOverlap;

	UPROPERTY(EditAnywhere)
	FVector startLocation;

	UPROPERTY(EditAnywhere)
	float impactForce = 100;

	UPROPERTY(EditAnywhere)
	class UQuadBallCharacterAnimInstance* characterAnimInstance;

	void MoveCharacterToTarget(FVector target);

	FVector oldTarget;

	UPROPERTY(EditAnywhere)
	float moveSpeedModifier = 1;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void SetReturned(bool inVal) { returned = inVal; };

	void ReturnBall();
	
};
