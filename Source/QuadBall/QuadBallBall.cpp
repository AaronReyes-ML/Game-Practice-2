// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadBallBall.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "QuadBallCourt.h"
#include "Engine.h"
#include "QuadBallCharacter.h"


// Sets default values
AQuadBallBall::AQuadBallBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ballRespawnerLocationA = CreateDefaultSubobject<USceneComponent>("Respawn Locale A");
	SetRootComponent(ballRespawnerLocationA);

	ballRespawnerLocationB = CreateDefaultSubobject<USceneComponent>("Respawn Locale B");
	ballRespawnerLocationB->SetupAttachment(ballRespawnerLocationA);
	ballRespawnerLocationB->SetRelativeLocation(FVector::ZeroVector);

	ballMesh = CreateDefaultSubobject<UStaticMeshComponent>("Ball mesh");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> obj0(TEXT("/Game/Meshes/Ball/StandardBall"));
	ballMesh->SetStaticMesh(obj0.Object);
	ballMesh->SetRelativeLocation(FVector::ZeroVector);

	ballMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	ballMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ballMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
	ballMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel6, ECR_Overlap);

	ballMesh->SetSimulatePhysics(true);

}

// Called when the game starts or when spawned
void AQuadBallBall::BeginPlay()
{
	Super::BeginPlay();

	if (court)
	{
		hasValidCourt = true;
	}
	ballMesh->OnComponentHit.AddDynamic(this, &AQuadBallBall::OnBallHit);
	ballMesh->OnComponentBeginOverlap.AddDynamic(this, &AQuadBallBall::OnOverlapBeginClean);

	playerBallResponsibility = court->GetBallStartSide();
	ResetBallLocation();
}

// Called every frame
void AQuadBallBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AQuadBallBall::GetBallLocation()
{
	return ballMesh->GetComponentLocation();
}

void AQuadBallBall::ResetBallLocation()
{
	// Move the ball to its serve location
	playerBallResponsibility = court->GetBallStartSide();
	ballMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	if (playerBallResponsibility == 0)
	{
		ballMesh->SetWorldLocation(ballRespawnerLocationA->GetComponentLocation());
	}
	else if (playerBallResponsibility == 1)
	{
		ballMesh->SetWorldLocation(ballRespawnerLocationB->GetComponentLocation());
	}
	// Stop the ball from moving
	ballMesh->SetSimulatePhysics(false);

	// Reset values
	clean = true;
	juggle = false;
	ballBounceCount = 0;
	ballPlayStatus = 0;
	court->ResetAllPlayerKicks();

	// Indicate who needs to serve
	currentBallSide = playerBallResponsibility;
	
	readyToServe = true;
}

void AQuadBallBall::ServeBall()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("SERVING"));

	ballPlayStatus = 1;
	ballMesh->SetSimulatePhysics(true);
	readyToServe = false;

	if (court->GetGameMode() == 0)
	{

	}
	else if (court->GetGameMode() == 1)
	{
		court->GetPlayerA()->SetAckServe(false);
	}
}

void AQuadBallBall::DoBallAction(FVector impulse)
{
	ballMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	ballMesh->AddImpulse(impulse * 75);
}

int AQuadBallBall::GetNewBallSide()
{
	return -1;
}

void AQuadBallBall::OnBallHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Increment Bounce Count
	if (court)
	{
		if (ballPlayStatus != 0)
		{
			if (OtherComp == court->GetCourtFloor() || (OtherComp != court->GetCourtNet() && ImpactInBounds(NormalImpulse)))
			{
				//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString("Hit floor"));
				// Hit floor
				//DrawDebugLine(GetWorld(), ballMesh->GetComponentLocation(), NormalImpulse, FColor::Cyan, true, 10);

				juggle = false;

				if (court->GetGameMode() == 1)
				{
					court->ResetTrainerReturn();
				}

				// A is responsible
				if (playerBallResponsibility == 0)
				{
					// Ball bounces on A side
					if (court->GetBallSide(this) == 0)
					{
						// Increase bounce count
						ballBounceCount += 1;
					}
					// Ball bounces on B side
					else if (court->GetBallSide(this) == 1)
					{
						// Transfer ball responsibility to B
						SetPlayerBallResponsibility(1);
						newServe = false;
						ballBounceCount = 1;
					}
				}
				else if (playerBallResponsibility == 1)
				{
					if (court->GetBallSide(this) == 1)
					{
						ballBounceCount += 1;
					}
					else if (court->GetBallSide(this) == 0)
					{
						// Transfer ball responsibility to A
						SetPlayerBallResponsibility(0);
						newServe = false;
						ballBounceCount = 1;
					}
				}
			}
			else if (OtherComp == court->GetCourtNet())
			{
				//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString("Hit net"));
				// Hit net
				if (newServe)
				{
					ballPlayStatus = 0;
					court->AwardPoint((playerBallResponsibility + 1) % 2);
				}
			}
			else if (OtherComp == court->GetCourtOBFloor())
			{
				//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString("Hit OB floor"));
				// Hit OB floor
				if (newServe)
				{
					ballPlayStatus = 0;
					court->AwardPoint((playerBallResponsibility + 1) % 2);
				}
				else
				{
					ballBounceCount += 1;
					juggle = false;
				}
			}

			if (ballPlayStatus == 1)
			{
				if (ballBounceCount > 2)
				{
					ballPlayStatus = 0;
					court->AwardPoint((playerBallResponsibility + 1) % 2);
				}
			}
			else if (ballPlayStatus == 2)
			{
				if (ballBounceCount > 1)
				{
					ballPlayStatus = 0;
					court->AwardPoint((playerBallResponsibility + 1) % 2);
				}
			}
		}
	}
}

void AQuadBallBall::OnOverlapBeginClean(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp == court->GetCleanSheet())
	{
		if (ballMesh->GetComponentLocation().Z >= court->GetMinimumSafeZLocation().Z && juggle == true)
		{
			clean = true;
		}
		else
		{
			clean = false;
		}
	}
}

void AQuadBallBall::SetPlayerBallResponsibility(int inVal)
{
	if (clean && ballPlayStatus != 0)
	{
		playerBallResponsibility = inVal;
		court->ResetAllPlayerKicks();
		juggle = false;

		if (ballPlayStatus != 0)
		{
			ballPlayStatus = 1;
		}
	}
	else if (!clean && ballPlayStatus != 0)
	{
		ballPlayStatus = 0;
		court->AwardPoint((playerBallResponsibility + 1) % 2);
	}
	else if (ballPlayStatus == 0)
	{
		playerBallResponsibility = inVal;
		court->ResetAllPlayerKicks();
		juggle = false;
	}
}

bool AQuadBallBall::ImpactInBounds(FVector impact)
{
	bool isInside = court->GetCourtInBoundary()->CalcBounds(FTransform()).GetBox().IsInside(ballMesh->GetComponentLocation() * FVector(1, 1, 0));
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString("Is inside?:") + FString::FromInt(isInside));
	return isInside;
}

