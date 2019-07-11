// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuadBallBall.generated.h"

UCLASS()
class QUADBALL_API AQuadBallBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuadBallBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ballMesh;

	UPROPERTY(EditAnywhere)
	class USceneComponent* ballRespawnerLocationA;
	UPROPERTY(EditAnywhere)
	class USceneComponent* ballRespawnerLocationB;

	UPROPERTY(EditAnywhere)
	class AQuadBallCourt* court;
	bool hasValidCourt = false;

	// 0 A, 1 B, -1 Not set
	int currentBallSide = -1;
	bool clean = true;

	UFUNCTION()
	void OnBallHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverlapBeginClean(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// -1 Pre play, 1 in play state 1 (can bounce twice), 2 in play state 2 (can bounce once), 0 play over
	bool readyToServe = false;
	bool newServe = true;
	int ballPlayStatus = -1;
	bool juggle = false;

	// Only play bounces count
	int ballBounceCount = -1;

	int playerBallResponsibility = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetBallLocation();
	void ResetBallLocation();

	void ServeBall();
	void DoBallAction(FVector inpulse);

	int GetNewBallSide();

	FORCEINLINE int GetCurrentBallSide() { return currentBallSide; };
	FORCEINLINE void SetBallSide(int newBallSide) { currentBallSide = newBallSide; };
	FORCEINLINE class UStaticMeshComponent* GetBallMesh() { return ballMesh; };
	
	FORCEINLINE bool GetReadyToServe() { return readyToServe; };
	FORCEINLINE int GetBallPlayStatus() { return ballPlayStatus; };
	FORCEINLINE void SetBallPlayStatus(int inVal) { ballPlayStatus = inVal; };
	FORCEINLINE int GetBallBounceCount() { return ballBounceCount; };
	FORCEINLINE void SetBallBounceCount(int inVal) { ballBounceCount = inVal; };

	FORCEINLINE int GetPlayerBallResponsibility() { return playerBallResponsibility; };
	void SetPlayerBallResponsibility(int inVal);

	FORCEINLINE bool GetJuggleStatus() { return juggle; };
	FORCEINLINE void SetJuggleStatus(bool inVal) { juggle = inVal; };

	FORCEINLINE bool GetClean() { return clean; };
	FORCEINLINE void SetClean(bool inVal) { clean = inVal; };

	FORCEINLINE bool GetNewServe() { return newServe; };
	FORCEINLINE void SetNewServe(bool inVal) { newServe = inVal; };

	bool ImpactInBounds(FVector impactZone);
	
};
