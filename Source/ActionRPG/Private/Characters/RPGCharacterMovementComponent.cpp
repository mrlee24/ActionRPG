// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/RPGCharacterMovementComponent.h"

#include "Utils/Libraries/RPGGameplayBPLibrary.h"


// Sets default values for this component's properties
URPGCharacterMovementComponent::URPGCharacterMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URPGCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

float URPGCharacterMovementComponent::GetMaxSpeed() const
{
	if (!URPGGameplayBPLibrary::IsRunningGameplayWorld(GetWorld()))
	{
		return Super::GetMaxSpeed();
	}

	switch (MovementMode)
	{
	case MOVE_Walking:
		return CurrentLocomotionState.MaxSpeed;
	case MOVE_NavWalking:
		return CurrentLocomotionState.MaxSpeed;
	case MOVE_Falling:
		return 600.f;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return CurrentLocomotionState.MaxSpeed;
	case MOVE_None:
	default:
		return 0.f;
	}
}

