// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "QuadBallPlayer.generated.h"

UCLASS()
class QUADBALL_API AQuadBallPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AQuadBallPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* playerMesh;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* playerCamera;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* ballInteractBound;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* ballAimVectorArm;

	void MoveRight(float axisVal);
	void MoveUp(float axisVal);
	void AimRight(float axisVal);
	void AimUp(float axisVal);

	float aimX = 0;
	float aimY = 0;

	bool hasActiveBall = false;
	bool ballInInteractRange = false;

	void ResetFromPlayer();

	UFUNCTION()
	void OnOverlapBeginBall(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEndBall(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void KickRight();
	void KickLeft();
	void Head();
	void DoBallKick();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> UIWidgetBase;
	class UQuadBallUI* UIWidget;
	
	UPROPERTY(EditAnywhere)
	int playerID = -1;

	bool hasKickedRight = false;
	bool hasKickedLeft = false;
	// 0 head or otherwise body, 1 right, 2 left
	int lastJuggle = -1;
	bool lastKickChanceUsed = false;

	UPROPERTY(EditAnywhere)
	int debugStatus = -1;

	bool ackServe = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	class AQuadBallBall* activeBall;
	UPROPERTY(EditAnywhere)
	class AQuadBallCourt* activeCourt;

	FORCEINLINE bool GetAckServe() { return ackServe; };
	FORCEINLINE void SetAckServe(bool inVal) { ackServe = inVal; };

	void ResetAllKicks();
	
};
