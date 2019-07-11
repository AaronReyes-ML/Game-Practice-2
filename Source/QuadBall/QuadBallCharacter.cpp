// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadBallCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
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
#include "Components/SkeletalMeshComponent.h"
#include "QuadBallCharacterAnimInstance.h"
#include "QuadBallGameInstance.h"

// Sets default values
AQuadBallCharacter::AQuadBallCharacter()
{
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

	playerRotatePoint = CreateDefaultSubobject<USceneComponent>("Player rotate point");
	playerRotatePoint->SetupAttachment(RootComponent);
	playerRotatePoint->SetRelativeLocation(FVector::ZeroVector);

	GetMesh()->SetRelativeLocation(FVector::ZeroVector);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetupAttachment(playerRotatePoint);

	playerCamera = CreateDefaultSubobject<UCameraComponent>("Player camera");
	playerCamera->SetupAttachment(RootComponent);
	playerCamera->SetRelativeLocation(FVector(-700, 0, 320));
	playerCamera->SetRelativeRotation(FRotator(-15, 0, 0));

	ballInteractBound = CreateDefaultSubobject<UBoxComponent>("Ball interact boundary");
	ballInteractBound->SetupAttachment(RootComponent);
	ballInteractBound->SetRelativeLocation(FVector(35,0,-45));
	ballInteractBound->SetBoxExtent(FVector(45, 60, 43));

	ballInteractBoundHead = CreateDefaultSubobject<UBoxComponent>("Ball interact boundary for head");
	ballInteractBoundHead->SetupAttachment(RootComponent);
	ballInteractBoundHead->SetRelativeLocation(FVector::ZeroVector);
	ballInteractBoundHead->SetRelativeLocation(FVector(5, 0, 70));
	ballInteractBoundHead->SetBoxExtent(FVector(32, 32, 24));

	ballInteractBoundBodyBlock = CreateDefaultSubobject<UBoxComponent>("Ball interact boundary for body");
	ballInteractBoundBodyBlock->SetupAttachment(RootComponent);
	ballInteractBoundBodyBlock->SetRelativeLocation(FVector::ZeroVector);
	ballInteractBoundBodyBlock->SetRelativeLocation(FVector(35, 0, 0));
	ballInteractBoundBodyBlock->SetBoxExtent(FVector(32, 32, 90));

	ballInteractBound->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	ballInteractBound->SetCollisionResponseToAllChannels(ECR_Ignore);
	ballInteractBound->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Overlap);

	ballInteractBoundHead->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	ballInteractBoundHead->SetCollisionResponseToAllChannels(ECR_Ignore);
	ballInteractBoundHead->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Overlap);

	ballInteractBoundBodyBlock->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	ballInteractBoundBodyBlock->SetCollisionResponseToAllChannels(ECR_Ignore);
	ballInteractBoundBodyBlock->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Overlap);
}

// Called when the game starts or when spawned
void AQuadBallCharacter::BeginPlay()
{
	Super::BeginPlay();

	gameInstance = Cast<UQuadBallGameInstance>(GetGameInstance());

	if (gameInstance)
	{
		if (playerID == 0)
		{
			playerSourceMesh = gameInstance->playerAMesh;
		}
		else
		{
			playerSourceMesh = gameInstance->playerBMesh;
		}
		GetMesh()->SetSkeletalMesh(playerSourceMesh);
	}

	UIWidget = Cast<UQuadBallUI>(CreateWidget<UUserWidget>(GetWorld(), UIWidgetBase));
	if (UIWidget)
	{
		UIWidget->AddToViewport();
	}

	ballInteractBound->OnComponentBeginOverlap.AddDynamic(this, &AQuadBallCharacter::OnOverlapBeginBall);
	ballInteractBound->OnComponentEndOverlap.AddDynamic(this, &AQuadBallCharacter::OnOverlapEndBall);

	ballInteractBoundHead->OnComponentBeginOverlap.AddDynamic(this, &AQuadBallCharacter::OnOverlapBeginBallHead);
	ballInteractBoundHead->OnComponentEndOverlap.AddDynamic(this, &AQuadBallCharacter::OnOverlapEndBallHead);

	ballInteractBoundBodyBlock->OnComponentBeginOverlap.AddDynamic(this, &AQuadBallCharacter::OnOverlapBeginBodyBlock);
	ballInteractBoundBodyBlock->OnComponentEndOverlap.AddDynamic(this, &AQuadBallCharacter::OnOverlapEndBallBodyBlock);

	if (activeBall)
	{
		hasActiveBall = true;
	}

	if (GetMesh())
	{
		characterAnimInstance = Cast<UQuadBallCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	}

	debugStatus = activeCourt->GetDebugStatus();
	
}

// Called every frame
void AQuadBallCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UIWidget->SetAimerLocation(aimX, aimY);	

	if (isCharging)
	{
		if (chargeAmount < maxCharge && isAscending)
		{ 
			chargeAmount += .8 * DeltaTime;
			if (chargeAmount >= maxCharge)
			{
				isAscending = false;
				chargeAmount = maxCharge;
			}
		}
		else if (chargeAmount > minCharge && !isAscending)
		{
			chargeAmount -= .8 * DeltaTime;
			if (chargeAmount <= minCharge)
			{
				isAscending = true;
				chargeAmount = minCharge;
			}
		}
	}


	if (debugStatus > 0)
	{
		if (debugStatus > 2)
		{
			// Input
			GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, FString("Right Input: ") + FString::SanitizeFloat(right)); FScreenMessageString* m0 = GEngine->ScreenMessages.Find(0); m0->TextScale.X = m0->TextScale.Y = 1.0f;
			GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, FString("Up Input: ") + FString::SanitizeFloat(forward)); FScreenMessageString* m1 = GEngine->ScreenMessages.Find(1); m1->TextScale.X = m1->TextScale.Y = 1.0f;
			GEngine->AddOnScreenDebugMessage(2, 1, FColor::Cyan, FString("Aim Right Input: ") + FString::SanitizeFloat(aimX)); FScreenMessageString* m2 = GEngine->ScreenMessages.Find(2); m2->TextScale.X = m2->TextScale.Y = 1.0f;
			GEngine->AddOnScreenDebugMessage(3, 1, FColor::Cyan, FString("Aim Up Input: ") + FString::SanitizeFloat(aimY)); FScreenMessageString* m3 = GEngine->ScreenMessages.Find(3); m3->TextScale.X = m3->TextScale.Y = 1.0f;
			//DrawDebugLine(GetWorld(), GetMesh()->GetComponentLocation(), GetMesh()->GetComponentLocation() + FVector(100, 100 * aimX, -100 * aimY), FColor::Cyan, false);

			// Ball
			if (activeBall)
			{
				DrawDebugLine(GetWorld(), GetMesh()->GetComponentLocation(), activeBall->GetBallLocation(), FColor::Blue, false);
			}

			FVector rightFootDirection = (GetMesh()->GetSocketLocation(FName("RightFootSocket")) + (FVector(-aimY * .6, aimX, (1.f - FMath::Abs(aimY* .6))) * 100));
			DrawDebugLine(GetWorld(), GetMesh()->GetSocketLocation(FName("RightFootSocket")), rightFootDirection, FColor::Cyan, false);
			FVector leftFootDirection = (GetMesh()->GetSocketLocation(FName("LeftFootSocket")) + (FVector(-aimY * .6, aimX, (1.f - FMath::Abs(aimY* .6))) * 100));
			DrawDebugLine(GetWorld(), GetMesh()->GetSocketLocation(FName("LeftFootSocket")), leftFootDirection, FColor::Cyan, false);
			FVector headDirection = (GetMesh()->GetSocketLocation(FName("HeadTop_EndSocket")) + (FVector(-aimY * .6, aimX, (1.f - FMath::Abs(aimY* .6))) * 100));
			DrawDebugLine(GetWorld(), GetMesh()->GetSocketLocation(FName("HeadTop_EndSocket")), headDirection, FColor::Cyan, false);
			FVector kneeDirection = (GetMesh()->GetSocketLocation(FName("RightLegSocket")) + (FVector(-aimY * .6 , aimX, (1.f - FMath::Abs(aimY* .6))) * 100));
			DrawDebugLine(GetWorld(), GetMesh()->GetSocketLocation(FName("RightLegSocket")), kneeDirection, FColor::Cyan, false);
		}

		if (debugStatus > 1)
		{
			GEngine->AddOnScreenDebugMessage(4, 1, FColor::Cyan, FString("Ball in interact range: ") + FString::FromInt(ballInInteractRange)); FScreenMessageString* m4 = GEngine->ScreenMessages.Find(4); m4->TextScale.X = m4->TextScale.Y = 1.0f;
			GEngine->AddOnScreenDebugMessage(5, 1, FColor::Cyan, FString("Ball Side: ") + FString::FromInt(activeCourt->GetBallSide(activeBall))); FScreenMessageString* m5 = GEngine->ScreenMessages.Find(5); m5->TextScale.X = m5->TextScale.Y = 1.0f;
			GEngine->AddOnScreenDebugMessage(6, 1, FColor::Cyan, FString("Ball Play Status: ") + FString::FromInt(activeBall->GetBallPlayStatus())); FScreenMessageString* m6 = GEngine->ScreenMessages.Find(6); m6->TextScale.X = m6->TextScale.Y = 1.0f;
			GEngine->AddOnScreenDebugMessage(13, 1, FColor::Cyan, FString("Ball Serve Ready?: ") + FString::FromInt(activeBall->GetReadyToServe()) + FString(", Ball New Serve?: ") + FString::FromInt(activeBall->GetNewServe())); FScreenMessageString* m13 = GEngine->ScreenMessages.Find(13); m13->TextScale.X = m13->TextScale.Y = 1.0f;
			GEngine->AddOnScreenDebugMessage(7, 1, FColor::Cyan, FString("Ball Bounce Count: ") + FString::FromInt(activeBall->GetBallBounceCount())); FScreenMessageString* m7 = GEngine->ScreenMessages.Find(7); m7->TextScale.X = m7->TextScale.Y = 1.0f;
			GEngine->AddOnScreenDebugMessage(12, 1, FColor::Cyan, FString("Ball Clean?: ") + FString::FromInt(activeBall->GetClean())); FScreenMessageString* m12 = GEngine->ScreenMessages.Find(12); m12->TextScale.X = m12->TextScale.Y = 1.0f;
			GEngine->AddOnScreenDebugMessage(10, 1, FColor::Cyan, FString("Ball Juggled?: ") + FString::FromInt(activeBall->GetJuggleStatus())); FScreenMessageString* m10 = GEngine->ScreenMessages.Find(10); m10->TextScale.X = m10->TextScale.Y = 1.0f;
		}

		if (debugStatus > 0)
		{
			GEngine->AddOnScreenDebugMessage(9, 1, FColor::Cyan, FString("Player A Score: ") + FString::FromInt(activeCourt->GetPlayerAScore()) + FString(", Player B Score: ") + FString::FromInt(activeCourt->GetPlayerBScore())); FScreenMessageString* m9 = GEngine->ScreenMessages.Find(9); m9->TextScale.X = m9->TextScale.Y = 1.0f;
		}
		if (debugStatus > 2)
		{
			// Game
			GEngine->AddOnScreenDebugMessage(8, 1, FColor::Cyan, FString("Player Responsibility: ") + FString::FromInt(activeBall->GetPlayerBallResponsibility())); FScreenMessageString* m8 = GEngine->ScreenMessages.Find(8); m8->TextScale.X = m8->TextScale.Y = 1.0f;
			GEngine->AddOnScreenDebugMessage(9, 1, FColor::Cyan, FString("Player A Score: ") + FString::FromInt(activeCourt->GetPlayerAScore()) + FString(", Player B Score: ") + FString::FromInt(activeCourt->GetPlayerBScore())); FScreenMessageString* m9 = GEngine->ScreenMessages.Find(9); m9->TextScale.X = m9->TextScale.Y = 1.0f;

			// Player
			GEngine->AddOnScreenDebugMessage(11, 1, FColor::Cyan, FString("Kick R: ") + FString::FromInt(hasKickedRight) + FString(", Kick L: ") + FString::FromInt(hasKickedLeft) + FString(", Handstand R: ") + FString::FromInt(hasHandstandRight) + FString(", Handstand L: ") + FString::FromInt(hasHandstandLeft) + FString(", Last Juggle: " + FString::FromInt(lastJuggle) + FString(", Last Kick Chance Used?: " + FString::FromInt(lastKickChanceUsed) + FString(", Last Hand Chance Used?: " + FString::FromInt(lastHandChanceUsed))))); FScreenMessageString* m11 = GEngine->ScreenMessages.Find(11); m11->TextScale.X = m11->TextScale.Y = 1.0f;
			GEngine->AddOnScreenDebugMessage(16, 1, FColor::Cyan, FString("Charge Amount: ") + FString::SanitizeFloat(chargeAmount)); FScreenMessageString* m16 = GEngine->ScreenMessages.Find(16); m16->TextScale.X = m16->TextScale.Y = 1.0f;
			GEngine->AddOnScreenDebugMessage(14, 1, FColor::Cyan, FString("Player Ack Serve?: ") + FString::FromInt(ackServe)); FScreenMessageString* m14 = GEngine->ScreenMessages.Find(14); m14->TextScale.X = m14->TextScale.Y = 1.0f;
			//GEngine->AddOnScreenDebugMessage(15, 1, FColor::Cyan, FString("Handstand Interp: ") + FString::FromInt(handstandInterp) + FString(", Rotate Point Roll: ") + FString::SanitizeFloat(playerRotatePoint->GetComponentRotation().Roll)); FScreenMessageString* m15 = GEngine->ScreenMessages.Find(15); m15->TextScale.X = m15->TextScale.Y = 1.0f;
		}
	}

}

void AQuadBallCharacter::Pause()
{
	if (isPaused)
	{
		UIWidget->HidePause();
		isPaused = false;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
		APlayerController* menuController = GetWorld()->GetFirstPlayerController();
		menuController->bShowMouseCursor = false;
		menuController->bEnableClickEvents = false;
		menuController->bEnableMouseOverEvents = false;
	}
	else
	{
		UIWidget->ShowPause();
		isPaused = true;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0);
		APlayerController* menuController = GetWorld()->GetFirstPlayerController();
		menuController->bShowMouseCursor = true;
		menuController->bEnableClickEvents = true;
		menuController->bEnableMouseOverEvents = true;
	}
}

// Called to bind functionality to input
void AQuadBallCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Reset", IE_Pressed, this, &AQuadBallCharacter::ResetFromPlayer);
	PlayerInputComponent->BindAction("KickR", IE_Pressed, this, &AQuadBallCharacter::Charge);
	PlayerInputComponent->BindAction("KickL", IE_Pressed, this, &AQuadBallCharacter::Charge);
	PlayerInputComponent->BindAction("Head", IE_Pressed, this, &AQuadBallCharacter::Charge);
	PlayerInputComponent->BindAction("BodyBlock", IE_Pressed, this, &AQuadBallCharacter::BodyBlock);
	PlayerInputComponent->BindAction("Switch", IE_Pressed, this, &AQuadBallCharacter::SwitchMode);
	PlayerInputComponent->BindAction("DebugUp", IE_Pressed, this, &AQuadBallCharacter::DebugUp);
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AQuadBallCharacter::Pause);

	PlayerInputComponent->BindAction("KickR", IE_Released, this, &AQuadBallCharacter::KickRight);
	PlayerInputComponent->BindAction("KickL", IE_Released, this, &AQuadBallCharacter::KickLeft);
	PlayerInputComponent->BindAction("Head", IE_Released, this, &AQuadBallCharacter::Head);

	PlayerInputComponent->BindAxis("Right", this, &AQuadBallCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Up", this, &AQuadBallCharacter::MoveUp);
	PlayerInputComponent->BindAxis("AimR", this, &AQuadBallCharacter::AimRight);
	PlayerInputComponent->BindAxis("AimU", this, &AQuadBallCharacter::AimUp);

}

void AQuadBallCharacter::Charge()
{
	if (!isCharging)
	{
		isCharging = true;
	}
}

void AQuadBallCharacter::CancelCharge()
{
	isCharging = false;
	chargeAmount = 1.f;
}

void AQuadBallCharacter::DebugUp()
{
	debugStatus = activeCourt->IncrementDebugStatus();
}

void AQuadBallCharacter::SwitchMode()
{
	if (handstandInterp == 0)
	{
		if (handStand)
		{
			characterAnimInstance->HandstandOff();
			handStand = false;
			//handstandInterp = -1;
			playerRotatePoint->SetRelativeRotation(FRotator(0, -90, 0));

		}
		else
		{
			characterAnimInstance->HandstandOn();
			handStand = true;
			//handstandInterp = 1;
			playerRotatePoint->SetRelativeRotation(FRotator(0, 90, 180));
		}
	}
}

void AQuadBallCharacter::MoveRight(float axisVal)
{
	if ((Controller != NULL) && (axisVal != 0.0f))
	{
		// find out which way is right
		FVector direction = GetCapsuleComponent()->GetForwardVector().RotateAngleAxis(90, FVector(0, 0, 1));

		AddMovementInput(direction * axisVal, 1, false);
		//GetMesh()->AddRelativeLocation(direction * axisVal * 10);
	}
}

void AQuadBallCharacter::MoveUp(float axisVal)
{
	if ((Controller != NULL) && (axisVal != 0.0f))
	{
		// find out which way is right
		FVector direction = GetCapsuleComponent()->GetForwardVector();

		AddMovementInput(direction * axisVal, 1, false);
	}
}

void AQuadBallCharacter::AimRight(float axisVal)
{
	if ((Controller != NULL) && (axisVal != 0.0f))
	{
		float val = FMath::Abs(axisVal);

		if (axisVal > 0)
		{
			if (aimX <= 1)
			{
				aimX += val / 50;
			}
		}
		else if (axisVal < 0)
		{
			if (aimX >= -1)
			{
				aimX -= val / 50;
			}
		}

		aimX > 1 ? aimX = 1 : NULL;
		aimX < -1 ? aimX = -1 : NULL;
	}
}

void AQuadBallCharacter::AimUp(float axisVal)
{
	if ((Controller != NULL) && (axisVal != 0.0f))
	{

		float val = FMath::Abs(axisVal);
		if (axisVal > 0)
		{
			if (aimY <= 1)
			{
				aimY -= val / 50;
			}
		}
		else if (axisVal < 0)
		{
			if (aimY >= -1)
			{
				aimY += val / 50;
			}
		}

		aimY > 1 ? aimY = 1 : NULL;
		aimY < -1 ? aimY = -1 : NULL;
	}
}

void AQuadBallCharacter::ResetFromPlayer()
{
	//Do player Reset
	if (debugStatus == 0 || activeBall->GetBallPlayStatus() == 0)
	{
		activeBall->ResetBallLocation();
	}

}

void AQuadBallCharacter::KickRight()
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
		characterAnimInstance->RightKick();
		// Ball is not the current player's responsibility so must change it to be so
		// Also must set ball back to 2 bounce status
		if (playerID != activeBall->GetPlayerBallResponsibility())
		{
			activeBall->SetPlayerBallResponsibility(playerID);
			activeBall->SetBallPlayStatus(1);
		}

		if (!handStand)
		{
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
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE JUGGLE RIGHT"));
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
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE RIGHT"));
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
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE JUGGLE RIGHT"));
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
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE RIGHT"));
						// Award Point To Other Player
						activeBall->SetBallPlayStatus(0);
						activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
					}
				}
			}
		}
		else
		{
			// Ball is in 2 bounce status
			if (activeBall->GetBallPlayStatus() == 1)
			{
				// Juggle hand
				if (activeBall->GetJuggleStatus())
				{
					if (lastJuggle != 3)
					{
						lastJuggle = 3;
					}
					// Double Juggle Handstand Right
					else
					{
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE JUGGLE HANDSTAND RIGHT"));
						// Award Point To Other Player
						activeBall->SetBallPlayStatus(0);
						activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
					}
				}
				else // Non juggle handstand
				{
					if (!hasHandstandRight)
					{
						hasHandstandRight = true;
						lastJuggle = 3;
						activeBall->SetBallPlayStatus(2);
						if (!lastHandChanceUsed)
						{
							activeBall->SetBallBounceCount(0);
							lastHandChanceUsed = true;
						}
					}
					// Double Hand Right
					else
					{
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE HAND RIGHT"));
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
					if (lastJuggle != 3)
					{
						lastJuggle = 3;
					}
					// Double Juggle Right
					else
					{
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE JUGGLE HAND RIGHT"));
						// Award Point To Other Player
						activeBall->SetBallPlayStatus(0);
						activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
					}
				}
				else // Non juggle kick
				{
					if (!hasHandstandRight)
					{
						hasHandstandRight = true;
						lastJuggle = 3;
						activeBall->SetBallPlayStatus(2);
						if (!lastHandChanceUsed)
						{
							activeBall->SetBallBounceCount(0);
							lastHandChanceUsed = true;
						}
					}
					// Double kick right
					else
					{
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE HAND RIGHT"));
						// Award Point To Other Player
						activeBall->SetBallPlayStatus(0);
						activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
					}
				}
			}
		}


		// If the Ball is being hit from the opposite site, it is not clean until it passes back over
		if (activeCourt->GetBallSide(activeBall) != playerID)
		{
			activeBall->SetClean(false);
		}

		FVector ballImpuseDirection = (GetMesh()->GetSocketLocation(FName("RightFootSocket")) + (FVector(-aimY, aimX, (1.f - FMath::Abs(aimY* .6))) * 100)) - GetMesh()->GetSocketLocation(FName("RightFootSocket"));
		ballImpuseDirection.Normalize();
		activeBall->DoBallAction(ballImpuseDirection * 100 * chargeAmount);
		activeBall->SetJuggleStatus(true);
	}
	CancelCharge();
}

void AQuadBallCharacter::KickLeft()
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
		characterAnimInstance->LeftKick();

		// Ball is not the current player's responsibility so must change it to be so
		// Also must set ball back to 2 bounce status
		if (playerID != activeBall->GetPlayerBallResponsibility())
		{
			activeBall->SetPlayerBallResponsibility(playerID);
			activeBall->SetBallPlayStatus(1);
		}

		if (!handStand)
		{
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
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE JUGGLE LEFT"));
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
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE LEFT"));
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
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE JUGGLE LEFT"));
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
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE LEFT"));
						// Award Point To Other Player
						activeBall->SetBallPlayStatus(0);
						activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
					}
				}
			}
		}
		else
		{
			// Ball is in 2 bounce status
			if (activeBall->GetBallPlayStatus() == 1)
			{
				// Juggle hand
				if (activeBall->GetJuggleStatus())
				{
					if (lastJuggle != 4)
					{
						lastJuggle = 4;
					}
					// Double Juggle Handstand Left
					else
					{
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE JUGGLE HANDSTAND LEFT"));
						// Award Point To Other Player
						activeBall->SetBallPlayStatus(0);
						activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
					}
				}
				else // Non juggle handstand
				{
					if (!hasHandstandLeft)
					{
						hasHandstandLeft = true;
						lastJuggle = 4;
						activeBall->SetBallPlayStatus(2);
						if (!lastHandChanceUsed)
						{
							activeBall->SetBallBounceCount(0);
							lastHandChanceUsed = true;
						}
					}
					// Double Hand Right
					else
					{
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE HAND LEFT"));
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
					if (lastJuggle != 4)
					{
						lastJuggle = 4;
					}
					// Double Juggle Left
					else
					{
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE JUGGLE HAND LEFT"));
						// Award Point To Other Player
						activeBall->SetBallPlayStatus(0);
						activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
					}
				}
				else // Non juggle kick
				{
					if (!hasHandstandLeft)
					{
						hasHandstandLeft = true;
						lastJuggle = 4;
						activeBall->SetBallPlayStatus(2);
						if (!lastHandChanceUsed)
						{
							activeBall->SetBallBounceCount(0);
							lastHandChanceUsed = true;
						}
					}
					// Double kick left
					else
					{
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("DOUBLE HAND LEFT"));
						// Award Point To Other Player
						activeBall->SetBallPlayStatus(0);
						activeCourt->AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2);
					}
				}
			}
		}


		// If the Ball is being hit from the opposite site, it is not clean until it passes back over
		if (activeCourt->GetBallSide(activeBall) != playerID)
		{
			activeBall->SetClean(false);
		}

		FVector ballImpuseDirection = (GetMesh()->GetSocketLocation(FName("LeftFootSocket")) + (FVector(-aimY, aimX, (1.f - FMath::Abs(aimY* .6))) * 100)) - GetMesh()->GetSocketLocation(FName("LeftFootSocket"));
		ballImpuseDirection.Normalize();
		activeBall->DoBallAction(ballImpuseDirection * 100 * chargeAmount);
		activeBall->SetJuggleStatus(true);
	}
	CancelCharge();
}

void AQuadBallCharacter::Head()
{
	if (ballInInteractRangeHead)
	{
		characterAnimInstance->Head();
		// Ball is not the current player's responsibility so must change it to be so
		// Also must set ball back to 2 bounce status
		if (playerID != activeBall->GetPlayerBallResponsibility())
		{
			activeBall->SetPlayerBallResponsibility(playerID);
			activeBall->SetBallPlayStatus(1);
		}

		lastJuggle = 0;
		FVector ballImpuseDirection = (GetMesh()->GetSocketLocation(FName("HeadTop_EndSocket")) + (FVector(-aimY, aimX, (1.f - FMath::Abs(aimY * .6))) * 100)) - GetMesh()->GetSocketLocation(FName("HeadTop_EndSocket"));
		ballImpuseDirection.Normalize();
		activeBall->DoBallAction(ballImpuseDirection * 65 * chargeAmount);
		activeBall->SetJuggleStatus(true);
	}
	else if (ballInInteractRange)
	{
		characterAnimInstance->Knee();
		// Ball is not the current player's responsibility so must change it to be so
		// Also must set ball back to 2 bounce status
		if (playerID != activeBall->GetPlayerBallResponsibility())
		{
			activeBall->SetPlayerBallResponsibility(playerID);
			activeBall->SetBallPlayStatus(1);
		}
		lastJuggle = 0;
		FVector ballImpuseDirection = (GetMesh()->GetSocketLocation(FName("RightLegSocket")) + (FVector(-aimY, aimX, (1.f - FMath::Abs(aimY* .6))) * 100)) - GetMesh()->GetSocketLocation(FName("RightLegSocket"));
		ballImpuseDirection.Normalize();
		activeBall->DoBallAction(ballImpuseDirection * 40 * chargeAmount);
		activeBall->SetJuggleStatus(true);
	}
	CancelCharge();
}

void AQuadBallCharacter::BodyBlock()
{
	if (ballInBodyBlockRange)
	{
		// Ball is not the current player's responsibility so must change it to be so
		// Also must set ball back to 2 bounce status
		if (playerID != activeBall->GetPlayerBallResponsibility())
		{
			activeBall->SetPlayerBallResponsibility(playerID);
			activeBall->SetBallPlayStatus(1);
		}

		lastJuggle = 0;
		FVector ballImpuseDirection = GetMesh()->GetUpVector();
		ballImpuseDirection.Normalize();
		activeBall->DoBallAction(ballImpuseDirection * 10);
		activeBall->SetJuggleStatus(true);
	}
}

void AQuadBallCharacter::DoBallKick()
{

}

void AQuadBallCharacter::ResetAllKicks()
{
	hasKickedRight = false;
	hasKickedLeft = false;
	hasHandstandRight = false;
	hasHandstandLeft = false;
	lastJuggle = -1;
	lastKickChanceUsed = false;
	lastHandChanceUsed = false;
}

void AQuadBallCharacter::OnOverlapBeginBall(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ballInInteractRange = true;
}

void AQuadBallCharacter::OnOverlapEndBall(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ballInInteractRange = false;
}


void AQuadBallCharacter::OnOverlapBeginBallHead(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ballInInteractRangeHead = true;
}

void AQuadBallCharacter::OnOverlapEndBallHead(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ballInInteractRangeHead = false;
}

void AQuadBallCharacter::OnOverlapBeginBodyBlock(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ballInBodyBlockRange = true;
}


void AQuadBallCharacter::OnOverlapEndBallBodyBlock(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ballInBodyBlockRange = false;
}