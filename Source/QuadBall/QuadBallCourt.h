// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuadBallCourt.generated.h"

UCLASS()
class QUADBALL_API AQuadBallCourt : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuadBallCourt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* courtMesh;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* courtOBFloor;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* centerNetMesh;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* playerACourtBoundary;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* playerBCourtBoundary;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* courtInBoundary;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* cleanSheet;

	UPROPERTY(EditAnywhere)
	int ballStartSide = 0;
	UPROPERTY(EditAnywhere)
	int playerBallResponsibility = 0; // 0 A, 1 B

	int playerAScore = 0;
	int playerBScore = 0;

	UPROPERTY(EditAnywhere)
	class AQuadBallCharacter* playerA;
	UPROPERTY(EditAnywhere)
	class AQuadBallCharacter* playerB;

	UPROPERTY(EditAnywhere)
	class AQuadBallBall* ball;

	UPROPERTY(EditAnywhere)
	class AQuadBallAutoCharacter* trainer;

	UPROPERTY(EditAnywhere)
	class USceneComponent* minimumSafeZ;
	UPROPERTY(EditAnywhere)
	class USceneComponent* trainerAimLocation;

	UPROPERTY(EditAnywhere)
	int debugStatus = 0;

	// 0 PVP, 1 Training
	UPROPERTY(EditAnywhere)
	int gameMode = 1;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int GetBallSide(class AQuadBallBall* ball);
	FORCEINLINE int GetBallStartSide() { return ballStartSide; };

	FORCEINLINE class UStaticMeshComponent* GetCourtFloor() { return courtMesh; };
	FORCEINLINE class UStaticMeshComponent* GetCourtOBFloor() { return courtOBFloor; };
	FORCEINLINE class UStaticMeshComponent* GetCourtNet() { return centerNetMesh; };
	FORCEINLINE class UBoxComponent* GetCleanSheet() { return cleanSheet; };

	FORCEINLINE int GetPlayerAScore() { return playerAScore; };
	FORCEINLINE int GetPlayerBScore() { return playerBScore; };

	FORCEINLINE class AQuadBallCharacter* GetPlayerA() { return playerA; };
	void AwardPoint(int playerTarget);

	FORCEINLINE int GetGameMode() { return gameMode; };

	FORCEINLINE class UBoxComponent* GetCourtInBoundary() { return courtInBoundary; };

	FORCEINLINE int GetDebugStatus() { return debugStatus; };
	FORCEINLINE int IncrementDebugStatus() { return debugStatus = (debugStatus + 1) % 4; };

	void ResetAllPlayerKicks();

	FVector GetMinimumSafeZLocation();
	FVector GetTrainerAimLocation();

	void ResetTrainerReturn();
};
