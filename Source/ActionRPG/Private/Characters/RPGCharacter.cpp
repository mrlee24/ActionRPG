// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/RPGCharacter.h"

#include "Characters/RPGCharacterMovementComponent.h"


// Sets default values
ARPGCharacter::ARPGCharacter(const FObjectInitializer& objectInitializer) :
	Super(objectInitializer.SetDefaultSubobjectClass<URPGCharacterMovementComponent>(CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LocomotionComponent = CreateDefaultSubobject<URPGLocomotionComponent>(TEXT("Locomotion Component"));
}

// Called when the game starts or when spawned
void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	RPGCharacterMovementComponent = Cast<URPGCharacterMovementComponent>(GetCharacterMovement());
	LocomotionComponent->GetLocomotionStateNativeChanged().AddUObject(this, &ThisClass::HandleLocomotionStateChanged);
}

// Called every frame
void ARPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ARPGCharacter::HandleLocomotionStateChanged(const FRPGLocomotionState& newLocomotionState)
{
	RPGCharacterMovementComponent->CurrentLocomotionState = newLocomotionState;
}

