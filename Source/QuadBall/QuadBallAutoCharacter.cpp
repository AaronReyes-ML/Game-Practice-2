// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadBallAutoCharacter.h"
#include "QuadBallBall.h"
#include "QuadBallCourt.h"
#include "QuadBallCharacter.h"
#include "Engine.h"
#include "QuadBallCharacterAnimInstance.h"
#include "Components/BoxComponent.h"
#include "QuadBallGameInstance.h"


// Sets default values
AQuadBallAutoCharacter::AQuadBallAutoCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

	kickOverlap = CreateDefaultSubobject<UBoxComponent>("Auto character kick overlap");
	kickOverlap->SetupAttachment(RootComponent);
	kickOverlap->SetRelativeLocation(FVector::ZeroVector);

	kickOverlap->SetCollisionResponseToAllChannels(ECR_Ignore);
	kickOverlap->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
}

// Called when the game starts or when spawned
void AQuadBallAutoCharacter::BeginPlay()
{
	Super::BeginPlay();

	gameInstance = Cast<UQuadBallGameInstance>(GetGameInstance());

	if (gameInstance)
	{
		trainerSourceMesh = gameInstance->playerBMesh;

		GetMesh()->SetSkeletalMesh(trainerSourceMesh);
	}

	if (GetMesh())
	{
		characterAnimInstance = Cast<UQuadBallCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	}

	oldTarget = startLocation;
}

// Called every frame
void AQuadBallAutoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (activeBall->GetBallPlayStatus() != 0)
	{
		if (activeCourt->GetBallSide(activeBall) == 1)
		{
			FVector ballTraject = activeBall->GetBallMesh()->GetComponentVelocity().GetSafeNormal();
			//DrawDebugLine(GetWorld(), activeBall->GetBallMesh()->GetComponentLocation(), activeBall->GetBallMesh()->GetComponentLocation() + ballTraject * 1000, FColor::Green, false);

			FHitResult outH;
			GetWorld()->LineTraceSingleByChannel(outH, activeBall->GetBallMesh()->GetComponentLocation(), activeBall->GetBallMesh()->GetComponentLocation() + ballTraject * 1000, ECC_WorldDynamic);


			if (outH.IsValidBlockingHit())
			{
				if (outH.GetComponent() == activeCourt->GetCourtFloor() || outH.GetComponent() == activeCourt->GetCourtOBFloor())
				{
					//GEngine->AddOnScreenDebugMessage(103, 1, FColor::Green, FString("Moving to ball impact"));
					if (activeCourt->GetDebugStatus() > 2)
						DrawDebugLine(GetWorld(), outH.Location, outH.Location + FVector(0,0,1) * 1000, FColor::Green, false);

					oldTarget = outH.Location;
					MoveCharacterToTarget(outH.Location);
				}
			}
			else
			{
				if (!returned)
				{
					MoveCharacterToTarget(activeBall->GetBallMesh()->GetComponentLocation() * FVector(1, 1, 0));
				}
			}
		}
	}
	else
	{
		MoveCharacterToTarget(startLocation);
		//GEngine->AddOnScreenDebugMessage(103, 1, FColor::Green, FString("Moving to start"));
	}

	//FScreenMessageString* m103 = GEngine->ScreenMessages.Find(103); m103->TextScale.X = m103->TextScale.Y = 1.0f;

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
	else if (activeBall->GetBallPlayStatus() != 0 && activeBall->GetBallBounceCount() >= 1 && !returned && activeBall->GetPlayerBallResponsibility() == 1)
	{
		trainerStatus = 4;
		if (kickOverlap->IsOverlappingComponent(activeBall->GetBallMesh()))
		{
			ReturnBall();
		}
	}
	else
	{
		trainerStatus = 5;
	}

	//GEngine->AddOnScreenDebugMessage(101, 1, FColor::Green, FString("Trainer Status?: ") + FString::FromInt(trainerStatus)); FScreenMessageString* m101 = GEngine->ScreenMessages.Find(101); m101->TextScale.X = m101->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(102, 1, FColor::Green, FString("Trainer Returned?: ") + FString::FromInt(returned)); FScreenMessageString* m102 = GEngine->ScreenMessages.Find(102); m102->TextScale.X = m102->TextScale.Y = 1.0f;

}

// Called to bind functionality to input
void AQuadBallAutoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AQuadBallAutoCharacter::ReturnBall()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("TRAINER RETURNING"));
	FVector ballImpuseDirection = activeCourt->GetTrainerAimLocation() - activeBall->GetBallLocation();
	ballImpuseDirection.Normalize();
	activeBall->DoBallAction(ballImpuseDirection * impactForce);
	activeBall->SetJuggleStatus(true);

	characterAnimInstance->RightKick();

	returned = true;
}

void AQuadBallAutoCharacter::MoveCharacterToTarget(FVector target)
{
	target.Z = 110;
	FVector direction = target - GetCapsuleComponent()->GetComponentLocation();
	direction.Normalize();

	AddMovementInput(direction, moveSpeedModifier, false);
}

