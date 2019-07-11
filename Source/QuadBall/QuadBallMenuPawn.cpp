// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadBallMenuPawn.h"
#include "Camera/CameraComponent.h"
#include "QuadBallMenuUI.h"
#include "QuadBallGameInstance.h"


// Sets default values
AQuadBallMenuPawn::AQuadBallMenuPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create an orthographic camera (no perspective) and attach it to the boom
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));

}

// Called when the game starts or when spawned
void AQuadBallMenuPawn::BeginPlay()
{
	Super::BeginPlay();

	UIWidget = Cast<UQuadBallMenuUI>(CreateWidget<UUserWidget>(GetWorld(), UIWidgetBase));
	if (UIWidget)
	{
		UIWidget->AddToViewport();
	}
	gameInstance = Cast<UQuadBallGameInstance>(GetGameInstance());

	APlayerController* menuController = GetWorld()->GetFirstPlayerController();
	menuController->bShowMouseCursor = true;
	menuController->bEnableClickEvents = true;
	menuController->bEnableMouseOverEvents = true;
}

// Called every frame
void AQuadBallMenuPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AQuadBallMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

