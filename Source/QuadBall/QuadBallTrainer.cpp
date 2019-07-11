// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadBallTrainer.h"
#include "QuadBallBall.h"
#include "QuadBallCourt.h"
#include "QuadBallCharacter.h"
#include "Engine.h"


// Sets default values
AQuadBallTrainer::AQuadBallTrainer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AQuadBallTrainer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AQuadBallTrainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (activeBall->GetBallPlayStatus() == 0 && activeBall->GetPlayerBallResponsibility() == 1)
	{
		trainerStatus = 1;
		if (activeBall->GetReadyToServe())
		{
			trainerStatus = 2;
			if (activeCourt->GetPlayerA()->GetAckServe())
			{
				trainerStatus = 3;
				activeBall->ServeBall();
			}
		}
	}
	else if (activeBall->GetBallPlayStatus() !=0  && activeBall->GetBallBounceCount() >= 1 && !returned && activeBall->GetPlayerBallResponsibility() == 1)
	{
		trainerStatus = 4;
		ReturnBall();
	}
	else
	{
		trainerStatus = 5;
	}

	//GEngine->AddOnScreenDebugMessage(101, 1, FColor::Green, FString("Trainer Status?: ") + FString::FromInt(trainerStatus)); FScreenMessageString* m101 = GEngine->ScreenMessages.Find(101); m101->TextScale.X = m101->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(102, 1, FColor::Green, FString("Trainer Returned?: ") + FString::FromInt(returned)); FScreenMessageString* m102 = GEngine->ScreenMessages.Find(102); m102->TextScale.X = m102->TextScale.Y = 1.0f;
}

// Called to bind functionality to input
void AQuadBallTrainer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AQuadBallTrainer::ReturnBall()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("TRAINER RETURNING"));
	FVector ballImpuseDirection = activeCourt->GetTrainerAimLocation() - activeBall->GetBallLocation();
	ballImpuseDirection.Normalize();
	activeBall->DoBallAction(ballImpuseDirection * impactForce);
	activeBall->SetJuggleStatus(true);

	returned = true;
}
