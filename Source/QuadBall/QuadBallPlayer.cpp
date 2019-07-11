// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadBallPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine.h"
#include "QuadBallBall.h"
#include "QuadBallCourt.h"
#include "QuadBallUI.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AQuadBallPlayer::AQuadBallPlayer()
{
	AutoPossessPlayer = EAutoReceiveInput::Player0;

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	playerMesh = CreateDefaultSubobject<UStaticMeshComponent>("Player mesh");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> obj0(TEXT("/Game/Meshes/Player/StandardPlayer"));
	playerMesh->SetStaticMesh(obj0.Object);

	playerMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	playerMesh->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);

	SetRootComponent(playerMesh);
	playerMesh->SetRelativeLocation(FVector::ZeroVector);

	playerCamera = CreateDefaultSubobject<UCameraComponent>("Player camera");
	playerCamera->SetupAttachment(playerMesh);
	playerCamera->SetRelativeLocation(FVector(-700, 0, 320));
	playerCamera->SetRelativeRotation(FRotator(-15, 0, 0));

	ballInteractBound = CreateDefaultSubobject<UCapsuleComponent>("Ball interact boundary");
	ballInteractBound->SetupAttachment(playerMesh);
	ballInteractBound->SetRelativeLocation(FVector::ZeroVector);
	ballInteractBound->SetCapsuleSize(100, 50);

	ballInteractBound->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	ballInteractBound->SetCollisionResponseToAllChannels(ECR_Ignore);
	ballInteractBound->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Overlap);

	ballAimVectorArm = CreateDefaultSubobject<USpringArmComponent>("Ball vector arm");
	ballAimVectorArm->SetupAttachment(playerMesh);
	ballAimVectorArm->SetRelativeLocation(FVector::ZeroVector);
	ballAimVectorArm->SetRelativeRotation(FRotator(0, -180, 0));
	ballAimVectorArm->SetHiddenInGame(false);
}

// Called when the game starts or when spawned
void AQuadBallPlayer::BeginPlay()
{
	Super::BeginPlay();

	UIWidget = Cast<UQuadBallUI>(CreateWidget<UUserWidget>(GetWorld(), UIWidgetBase));
	if (UIWidget)
	{
		UIWidget->AddToViewport();
	}

	ballInteractBound->OnComponentBeginOverlap.AddDynamic(this, &AQuadBallPlayer::OnOverlapBeginBall);
	ballInteractBound->OnComponentEndOverlap.AddDynamic(this, &AQuadBallPlayer::OnOverlapEndBall);

	if (activeBall)
	{
		hasActiveBall = true;
	}
	
}

// Called every frame
void AQuadBallPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

	UIWidget->SetAimerLocation(aimX, aimY);

	if (debugStatus == 0)
	{
		// Input
		//GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, FString("Right Input: ") + FString::SanitizeFloat(GetInputAxisValue("Right"))); FScreenMessageString* m0 = GEngine->ScreenMessages.Find(0); m0->TextScale.X = m0->TextScale.Y = 1.0f;
		//GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, FString("Up Input: ") + FString::SanitizeFloat(GetInputAxisValue("Up"))); FScreenMessageString* m1 = GEngine->ScreenMessages.Find(1); m1->TextScale.X = m1->TextScale.Y = 1.0f;
		//GEngine->AddOnScreenDebugMessage(2, 1, FColor::Cyan, FString("Aim Right Input: ") + FString::SanitizeFloat(aimX)); FScreenMessageString* m2 = GEngine->ScreenMessages.Find(2); m2->TextScale.X = m2->TextScale.Y = 1.0f;
		//GEngine->AddOnScreenDebugMessage(3, 1, FColor::Cyan, FString("Aim Up Input: ") + FString::SanitizeFloat(aimY)); FScreenMessageString* m3 = GEngine->ScreenMessages.Find(3); m3->TextScale.X = m3->TextScale.Y = 1.0f;
		//DrawDebugLine(GetWorld(), playerMesh->GetComponentLocation(), playerMesh->GetComponentLocation() + FVector(100, 100 * aimX, -100 * aimY), FColor::Cyan, false);

		// Ball
		//if (activeBall)
		//{
		//	DrawDebugLine(GetWorld(), playerMesh->GetComponentLocation(), activeBall->GetBallLocation(), FColor::Blue, false);
		//}
		GEngine->AddOnScreenDebugMessage(4, 1, FColor::Cyan, FString("Ball in interact range: ") + FString::FromInt(ballInInteractRange)); FScreenMessageString* m4 = GEngine->ScreenMessages.Find(4); m4->TextScale.X = m4->TextScale.Y = 1.0f;
		GEngine->AddOnScreenDebugMessage(5, 1, FColor::Cyan, FString("Ball Side: ") + FString::FromInt(activeCourt->GetBallSide(activeBall))); FScreenMessageString* m5 = GEngine->ScreenMessages.Find(5); m5->TextScale.X = m5->TextScale.Y = 1.0f;
		GEngine->AddOnScreenDebugMessage(6, 1, FColor::Cyan, FString("Ball Play Status: ") + FString::FromInt(activeBall->GetBallPlayStatus())); FScreenMessageString* m6 = GEngine->ScreenMessages.Find(6); m6->TextScale.X = m6->TextScale.Y = 1.0f;
		GEngine->AddOnScreenDebugMessage(13, 1, FColor::Cyan, FString("Ball Serve Ready?: ") + FString::FromInt(activeBall->GetReadyToServe())); FScreenMessageString* m13 = GEngine->ScreenMessages.Find(13); m13->TextScale.X = m13->TextScale.Y = 1.0f;
		GEngine->AddOnScreenDebugMessage(7, 1, FColor::Cyan, FString("Ball Bounce Count: ") + FString::FromInt(activeBall->GetBallBounceCount())); FScreenMessageString* m7 = GEngine->ScreenMessages.Find(7); m7->TextScale.X = m7->TextScale.Y = 1.0f;
		GEngine->AddOnScreenDebugMessage(12, 1, FColor::Cyan, FString("Ball Clean?: ") + FString::FromInt(activeBall->GetClean())); FScreenMessageString* m12 = GEngine->ScreenMessages.Find(12); m12->TextScale.X = m12->TextScale.Y = 1.0f;
		GEngine->AddOnScreenDebugMessage(10, 1, FColor::Cyan, FString("Ball Juggled?: ") + FString::FromInt(activeBall->GetJuggleStatus())); FScreenMessageString* m10 = GEngine->ScreenMessages.Find(10); m10->TextScale.X = m10->TextScale.Y = 1.0f;

		// Game
		GEngine->AddOnScreenDebugMessage(8, 1, FColor::Cyan, FString("Player Responsibility: ") + FString::FromInt(activeBall->GetPlayerBallResponsibility())); FScreenMessageString* m8 = GEngine->ScreenMessages.Find(8); m8->TextScale.X = m8->TextScale.Y = 1.0f;
		GEngine->AddOnScreenDebugMessage(9, 1, FColor::Cyan, FString("Player A Score: ") + FString::FromInt(activeCourt->GetPlayerAScore()) + FString(", Player B Score: ") + FString::FromInt(activeCourt->GetPlayerBScore())); FScreenMessageString* m9 = GEngine->ScreenMessages.Find(9); m9->TextScale.X = m9->TextScale.Y = 1.0f;

		// Player
		GEngine->AddOnScreenDebugMessage(11, 1, FColor::Cyan, FString("Kick R: ") + FString::FromInt(hasKickedRight) + FString(", Kick L: ") + FString::FromInt(hasKickedLeft) + FString(", Last Juggle: " + FString::FromInt(lastJuggle) + FString(", Last Kick Chance Used?: " + FString::FromInt(lastKickChanceUsed)))); FScreenMessageString* m11 = GEngine->ScreenMessages.Find(11); m11->TextScale.X = m11->TextScale.Y = 1.0f;
		GEngine->AddOnScreenDebugMessage(14, 1, FColor::Cyan, FString("Player Ack Serve?: ") + FString::FromInt(ackServe)); FScreenMessageString* m14 = GEngine->ScreenMessages.Find(14); m14->TextScale.X = m14->TextScale.Y = 1.0f;
	}
}

// Called to bind functionality to input
void AQuadBallPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Reset", IE_Pressed, this, &AQuadBallPlayer::ResetFromPlayer);
	PlayerInputComponent->BindAction("KickR", IE_Pressed, this, &AQuadBallPlayer::KickRight);
	PlayerInputComponent->BindAction("KickL", IE_Pressed, this, &AQuadBallPlayer::KickLeft);
	PlayerInputComponent->BindAction("Head", IE_Pressed, this, &AQuadBallPlayer::Head);

	PlayerInputComponent->BindAxis("Right", this, &AQuadBallPlayer::MoveRight);
	PlayerInputComponent->BindAxis("Up", this, &AQuadBallPlayer::MoveUp);
	PlayerInputComponent->BindAxis("AimR", this, &AQuadBallPlayer::AimRight);
	PlayerInputComponent->BindAxis("AimU", this, &AQuadBallPlayer::AimUp);
}

void AQuadBallPlayer::MoveRight(float axisVal)
{
	if ((Controller != NULL) && (axisVal != 0.0f))
	{
		// find out which way is right
		FVector direction = playerMesh->GetForwardVector().RotateAngleAxis(90, FVector(0, 0, 1));

		AddMovementInput(direction, 1, true);
		//playerMesh->AddRelativeLocation(direction * axisVal * 10);
	}
}

void AQuadBallPlayer::MoveUp(float axisVal)
{
	if ((Controller != NULL) && (axisVal != 0.0f))
	{
		// find out which way is right
		FVector direction = playerMesh->GetForwardVector();

		AddMovementInput(direction, 1, true);
	}
}

void AQuadBallPlayer::AimRight(float axisVal)
{
	if ((Controller != NULL) && (axisVal != 0.0f))
	{
		float val = FMath::Abs(axisVal);

		if (axisVal > 0)
		{
			if (aimX <= 1)
			{
				aimX += val / 100;
			}
		}
		else if (axisVal < 0)
		{
			if (aimX >= -1)
			{
				aimX -= val / 100;
			}
		}

		aimX > 1 ? aimX = 1 : NULL;
		aimX < -1 ? aimX = -1 : NULL;
	}
}

void AQuadBallPlayer::AimUp(float axisVal)
{
	if ((Controller != NULL) && (axisVal != 0.0f))
	{

		float val = FMath::Abs(axisVal);
		if (axisVal > 0)
		{
			if (aimY <= 1)
			{
				aimY -= val / 100;
			}
		}
		else if (axisVal < 0)
		{
			if (aimY >= -1)
			{
				aimY += val / 100;
			}
		}

		aimY > 1 ? aimY = 1 : NULL;
		aimY < -1 ? aimY = -1 : NULL;
	}
}

void AQuadBallPlayer::ResetFromPlayer()
{
	//Do player Reset
	if (debugStatus == 0 || activeBall->GetBallPlayStatus() == 0)
	{
		activeBall->ResetBallLocation();
	}

}

void AQuadBallPlayer::KickRight()
{
	if (activeBall->GetBallPlayStatus() == 0 && activeBall->GetReadyToServe())
	{
		if (activeBall->GetPlayerBallResponsibility() == playerID)
		{
			activeBall->ServeBall();
		}
		else
		{
			ackServe = true;
		}
	}
	else if (ballInInteractRange)
	{
		// Ball is not the current player's responsibility so must change it to be so
		// Also must set ball back to 2 bounce status
		if (playerID != activeBall->GetPlayerBallResponsibility())
		{
			activeBall->SetPlayerBallResponsibility(playerID);
			activeBall->SetBallPlayStatus(1);
		}

		// Ball is in 2 bounce status
		if (activeBall->GetBallPlayStatus() == 1)
		{
			// Juggle kick
			if (activeBall->GetJuggleStatus())
			{
				if (lastJuggle != 1)
				{
					lastJuggle = 1;
				}
				// Double Juggle Right
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE JUGGLE RIGHT"));
					// Award Point To Other Player
					activeBall->SetBallPlayStatus(0);
					activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
				}
			}
			else // Non juggle kick
			{
				if (!hasKickedRight)
				{
					hasKickedRight = true;
					lastJuggle = 1;
					activeBall->SetBallPlayStatus(2);
					if (!lastKickChanceUsed)
					{
						activeBall->SetBallBounceCount(0);
						lastKickChanceUsed = true;
					}
				}
				// Double Kick right
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE RIGHT"));
					// Award Point To Other Player
					activeBall->SetBallPlayStatus(0);
					activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
				}
			}
		}
		// Ball is in 1 bounce status
		else if (activeBall->GetBallPlayStatus() == 2)
		{
			// Juggle kick
			if (activeBall->GetJuggleStatus())
			{
				if (lastJuggle != 1)
				{
					lastJuggle = 1;
				}
				// Double Juggle Right
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE JUGGLE RIGHT"));
					// Award Point To Other Player
					activeBall->SetBallPlayStatus(0);
					activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
				}
			}
			else // Non juggle kick
			{
				if (!hasKickedRight)
				{
					hasKickedRight = true;
					lastJuggle = 1;
					activeBall->SetBallPlayStatus(2);
					if (!lastKickChanceUsed)
					{
						activeBall->SetBallBounceCount(0);
						lastKickChanceUsed = true;
					}
				}
				// Double kick right
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE RIGHT"));
					// Award Point To Other Player
					activeBall->SetBallPlayStatus(0);
					activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
				}
			}
		}

		// If the Ball is being hit from the opposite site, it is not clean until it passes back over
		if (activeCourt->GetBallSide(activeBall) != playerID)
		{
			activeBall->SetClean(false);
		}

		FVector ballImpuseDirection = (playerMesh->GetComponentLocation() + FVector(100, 100 * aimX, -100 * aimY) - playerMesh->GetComponentLocation());
		ballImpuseDirection.Normalize();
		activeBall->DoBallAction(ballImpuseDirection * 100);
		activeBall->SetJuggleStatus(true);
	}
}

void AQuadBallPlayer::KickLeft()
{
	if (activeBall->GetBallPlayStatus() == 0 && activeBall->GetReadyToServe())
	{
		if (activeBall->GetPlayerBallResponsibility() == playerID)
		{
			activeBall->ServeBall();
		}
		else
		{
			ackServe = true;
		}
	}
	else if (ballInInteractRange)
	{
		// Ball is not the current player's responsibility so must change it to be so
		// Also must set ball back to 2 bounce status
		if (playerID != activeBall->GetPlayerBallResponsibility())
		{
			activeBall->SetPlayerBallResponsibility(playerID);
			activeBall->SetBallPlayStatus(1);
		}

		// Ball is in 2 bounce status
		if (activeBall->GetBallPlayStatus() == 1)
		{
			// Juggle kick
			if (activeBall->GetJuggleStatus())
			{
				if (lastJuggle != 2)
				{
					lastJuggle = 2;
				}
				// Double Juggle Left
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE JUGGLE LEFT"));
					// Award Point To Other Player
					activeBall->SetBallPlayStatus(0);
					activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
				}
			}
			else // Non juggle kick
			{
				if (!hasKickedLeft)
				{
					hasKickedLeft = true;
					lastJuggle = 2;
					activeBall->SetBallPlayStatus(2);
					if (!lastKickChanceUsed)
					{
						activeBall->SetBallBounceCount(0);
						lastKickChanceUsed = true;
					}
				}
				// Double Kick Left
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE LEFT"));
					// Award Point To Other Player
					activeBall->SetBallPlayStatus(0);
					activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
				}
			}
		}
		// Ball is in 1 bounce status
		else if (activeBall->GetBallPlayStatus() == 2)
		{
			// Juggle kick
			if (activeBall->GetJuggleStatus())
			{
				if (lastJuggle != 2)
				{
					lastJuggle = 2;
				}
				// Double Juggle Left
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE JUGGLE LEFT"));
					// Award Point To Other Player
					activeBall->SetBallPlayStatus(0);
					activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
				}
			}
			else // Non juggle kick
			{
				if (!hasKickedLeft)
				{
					hasKickedLeft = true;
					lastJuggle = 2;
					activeBall->SetBallPlayStatus(2);
					if (!lastKickChanceUsed)
					{
						activeBall->SetBallBounceCount(0);
						lastKickChanceUsed = true;
					}
				}
				// Double kick Left
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE LEFT"));
					// Award Point To Other Player
					activeBall->SetBallPlayStatus(0);
					activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
				}
			}
		}

		// If the Ball is being hit from the opposite site, it is not clean until it passes back over
		if (activeCourt->GetBallSide(activeBall) != playerID)
		{
			activeBall->SetClean(false);
		}

		FVector ballImpuseDirection = (playerMesh->GetComponentLocation() + FVector(100, 100 * aimX, -100 * aimY) - playerMesh->GetComponentLocation());
		ballImpuseDirection.Normalize();
		activeBall->DoBallAction(ballImpuseDirection * 100);
		activeBall->SetJuggleStatus(true);
	}
}

void AQuadBallPlayer::Head()
{
	if (ballInInteractRange)
	{
		// Ball is not the current player's responsibility so must change it to be so
		// Also must set ball back to 2 bounce status
		if (playerID != activeBall->GetPlayerBallResponsibility())
		{
			activeBall->SetPlayerBallResponsibility(playerID);
			activeBall->SetBallPlayStatus(1);
		}

		lastJuggle = 0;
		FVector ballImpuseDirection = playerMesh->GetUpVector();
		ballImpuseDirection.Normalize();
		activeBall->DoBallAction(ballImpuseDirection * 100);
		activeBall->SetJuggleStatus(true);
	}
}

void AQuadBallPlayer::DoBallKick()
{

}

void AQuadBallPlayer::ResetAllKicks()
{
	hasKickedRight = false;
	hasKickedLeft = false;
	lastJuggle = -1;
	lastKickChanceUsed = false;
}

void AQuadBallPlayer::OnOverlapBeginBall(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ballInInteractRange = true;
}

void AQuadBallPlayer::OnOverlapEndBall(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ballInInteractRange = false;
}
