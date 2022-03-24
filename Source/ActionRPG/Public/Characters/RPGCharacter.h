// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/RPGAnimInstanceInterface.h"
#include "RPGCharacter.generated.h"

UCLASS()
class ACTIONRPG_API ARPGCharacter : public ACharacter,
									public IRPGAnimInstanceInterface
{
	GENERATED_BODY()

	
public:
	// Sets default values for this character's properties
	ARPGCharacter(const FObjectInitializer& objectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private: // IRPGAnimInstanceInterface
	void HandleLocomotionStateChanged(const struct FRPGLocomotionState& newLocomotionState);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	class URPGLocomotionComponent* LocomotionComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	class URPGCharacterMovementComponent* RPGCharacterMovementComponent = nullptr;
};
