// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadBallCourt.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "QuadBallBall.h"
#include "Engine.h"
#include "QuadBallCharacter.h"
#include "QuadBallAutoCharacter.h"


// Sets default values
AQuadBallCourt::AQuadBallCourt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	courtMesh = CreateDefaultSubobject<UStaticMeshComponent>("Court");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> obj0(TEXT("/Game/Meshes/Court/StandardCourt"));
	courtMesh->SetStaticMesh(obj0.Object);
	SetRootComponent(courtMesh);
	
	courtMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	courtOBFloor = CreateDefaultSubobject<UStaticMeshComponent>("Out of bounds floor");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> obj2(TEXT("/Game/Meshes/Court/StandardOBFloor"));
	courtOBFloor->SetStaticMesh(obj2.Object);

	courtOBFloor->SetupAttachment(courtMesh);
	courtOBFloor->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	courtOBFloor->SetRelativeLocation(FVector::ZeroVector);

	centerNetMesh = CreateDefaultSubobject<UStaticMeshComponent>("Center Mesh");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> obj1(TEXT("/Game/Meshes/Court/StandardNet"));
	centerNetMesh->SetStaticMesh(obj1.Object);

	centerNetMesh->SetupAttachment(courtMesh);
	centerNetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	centerNetMesh->SetRelativeLocation(FVector::ZeroVector);

	playerACourtBoundary = CreateDefaultSubobject<UBoxComponent>("Player A Boundary");
	
	playerACourtBoundary->SetupAttachment(courtMesh);
	playerACourtBoundary->SetRelativeLocation(FVector::ZeroVector);
	playerACourtBoundary->SetCollisionResponseToAllChannels(ECR_Ignore);
	playerACourtBoundary->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	playerBCourtBoundary = CreateDefaultSubobject<UBoxComponent>("Player B Boundary");

	playerBCourtBoundary->SetupAttachment(courtMesh);
	playerBCourtBoundary->SetRelativeLocation(FVector::ZeroVector);
	playerBCourtBoundary->SetCollisionResponseToAllChannels(ECR_Ignore);
	playerBCourtBoundary->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	courtInBoundary = CreateDefaultSubobject<UBoxComponent>("Court in Boundary");
	courtInBoundary->SetupAttachment(courtMesh);
	courtInBoundary->SetRelativeLocation(FVector::ZeroVector);
	courtInBoundary->SetCollisionResponseToAllChannels(ECR_Ignore);

	cleanSheet = CreateDefaultSubobject<UBoxComponent>("Cleaner Sheet");

	cleanSheet->SetupAttachment(courtMesh);
	cleanSheet->SetRelativeLocation(FVector::ZeroVector);
	cleanSheet->SetCollisionResponseToAllChannels(ECR_Ignore);
	cleanSheet->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	minimumSafeZ = CreateDefaultSubobject<USceneComponent>("Minimum Safe Z");
	minimumSafeZ->SetupAttachment(courtMesh);
	minimumSafeZ->SetRelativeLocation(FVector::ZeroVector);

	trainerAimLocation = CreateDefaultSubobject<USceneComponent>("Trainer Aim Location");
	trainerAimLocation->SetupAttachment(courtMesh);
	trainerAimLocation->SetRelativeLocation(FVector::ZeroVector);
}

// Called when the game starts or when spawned
void AQuadBallCourt::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AQuadBallCourt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int AQuadBallCourt::GetBallSide(AQuadBallBall* ball)
{
	// TEST
	//GEngine->AddOnScreenDebugMessage(101, 1, FColor::Blue, FString("BALL OVERLAPPING A: ") + FString::FromInt(playerACourtBoundary->IsOverlappingComponent(ball->GetBallMesh()))); FScreenMessageString* m101 = GEngine->ScreenMessages.Find(101); m101->TextScale.X = m101->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(102, 1, FColor::Blue, FString("BALL OVERLAPPING B: ") + FString::FromInt(playerBCourtBoundary->IsOverlappingComponent(ball->GetBallMesh()))); FScreenMessageString* m102 = GEngine->ScreenMessages.Find(102); m102->TextScale.X = m102->TextScale.Y = 1.0f;

	if (playerACourtBoundary->IsOverlappingComponent(ball->GetBallMesh()))
	{

		return 0;
	}
	else if (playerBCourtBoundary->IsOverlappingComponent(ball->GetBallMesh()))
	{
		return 1;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("Ball side not found"));
		return -1;
	}

}

void AQuadBallCourt::AwardPoint(int playerTarget)
{
	if (!ball->GetNewServe())
	{
		if (playerTarget == 0)
		{
			playerAScore += 1;
			ballStartSide = 0;
			ball->SetNewServe(true);
		}
		else if (playerTarget == 1)
		{
			playerBScore += 1;
			ballStartSide = 1;
			ball->SetNewServe(true);
		}
	}
	else
	{
		if (playerTarget == 0)
		{
			ballStartSide = 1;
			ball->SetNewServe(false);
		}
		else if (playerTarget == 1)
		{
			ballStartSide = 0;
			ball->SetNewServe(false);
		}
	}
}

void AQuadBallCourt::ResetAllPlayerKicks()
{
	if (gameMode == 0)
	{
		if (playerA)
		{
			playerA->ResetAllKicks();
		}
		if (playerB)
		{
			playerB->ResetAllKicks();
		}
	}
	else if (gameMode == 1)
	{
		if (playerA)
		{
			playerA->ResetAllKicks();
		}
		if (trainer)
		{
			trainer->SetReturned(false);
		}
	}

}

FVector AQuadBallCourt::GetMinimumSafeZLocation()
{
	return minimumSafeZ->GetComponentLocation();
}

FVector AQuadBallCourt::GetTrainerAimLocation()
{
	return trainerAimLocation->GetComponentLocation();
}

void AQuadBallCourt::ResetTrainerReturn()
{
	trainer->SetReturned(false);
}

