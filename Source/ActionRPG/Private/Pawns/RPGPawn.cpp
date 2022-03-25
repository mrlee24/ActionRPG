// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/RPGPawn.h"

#include "Animations/Components/RPGLocomotionComponent.h"
#include "Pawns/RPGPawnMovementComponent.h"


// Sets default values
ARPGPawn::ARPGPawn()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RPGPawnMovementComponent = CreateDefaultSubobject<URPGPawnMovementComponent>(TEXT("Pawn Movement Component"));
	LocomotionComponent = CreateDefaultSubobject<URPGLocomotionComponent>(TEXT("Locomotion Component"));
}

// Called when the game starts or when spawned
void ARPGPawn::BeginPlay()
{
	Super::BeginPlay();

	LocomotionComponent->GetLocomotionStateNativeChanged().AddUObject(this, &ThisClass::HandleLocomotionStateChanged);		
}

// Called every frame
void ARPGPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARPGPawn::HandleLocomotionStateChanged(const FRPGLocomotionState& newLocomotionState)
{
	RPGPawnMovementComponent->CurrentLocomotionState = newLocomotionState;
}

