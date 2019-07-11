// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "QuadBallCharacter.generated.h"

UCLASS()
class QUADBALL_API AQuadBallCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AQuadBallCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class UQuadBallGameInstance* gameInstance;

	UPROPERTY(EditAnywhere)
	class USceneComponent* playerRotatePoint;

	UPROPERTY(EditAnywhere)
	class USkeletalMesh* playerSourceMesh;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* playerCamera;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* ballInteractBound;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* ballInteractBoundHead;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* ballInteractBoundBodyBlock;

	void MoveRight(float axisVal);
	void MoveUp(float axisVal);
	void AimRight(float axisVal);
	void AimUp(float axisVal);

	float aimX = 0;
	float aimY = 0;

	bool hasActiveBall = false;
	bool ballInInteractRange = false;
	bool ballInInteractRangeHead = false;
	bool ballInBodyBlockRange = false;

	bool handStand = false;

	void ResetFromPlayer();

	UFUNCTION()
	void OnOverlapBeginBall(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEndBall(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UFUNCTION()
	void OnOverlapBeginBallHead(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEndBallHead(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void OnOverlapBeginBodyBlock(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEndBallBodyBlock(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void KickRight();
	void KickLeft();
	void Head();
	void DoBallKick();
	void BodyBlock();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> UIWidgetBase;
	class UQuadBallUI* UIWidget;

	UPROPERTY(EditAnywhere)
		int playerID = -1;

	bool hasKickedRight = false;
	bool hasKickedLeft = false;
	bool hasHandstandRight = false;
	bool hasHandstandLeft = false;
	// 0 head or otherwise body, 1 right, 2 left
	int lastJuggle = -1;
	bool lastKickChanceUsed = false;
	bool lastHandChanceUsed = false;

	float chargeAmount = 1.f;
	float minCharge = 1.f;
	float maxCharge = 2.f;
	bool isCharging = false;
	bool isAscending = true;
	void Charge();
	void CancelCharge();

	int handstandInterp = 0;

	void SwitchMode();

	UPROPERTY(EditAnywhere)
		int debugStatus = -1;

	bool ackServe = false;

	UPROPERTY(EditAnywhere)
	class UQuadBallCharacterAnimInstance* characterAnimInstance;


	void DebugUp();
	bool isPaused = false;
	void Pause();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float forward = 0;
	float right = 0;

	UPROPERTY(EditAnywhere)
	class AQuadBallBall* activeBall;
	UPROPERTY(EditAnywhere)
	class AQuadBallCourt* activeCourt;

	FORCEINLINE bool GetAckServe() { return ackServe; };
	FORCEINLINE void SetAckServe(bool inVal) { ackServe = inVal; };

	void ResetAllKicks();
	
};
