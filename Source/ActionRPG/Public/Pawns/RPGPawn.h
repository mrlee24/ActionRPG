// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/RPGMovableInterface.h"
#include "RPGPawn.generated.h"

UCLASS()
class ACTIONRPG_API ARPGPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARPGPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void HandleLocomotionStateChanged(const struct FRPGLocomotionState& newLocomotionState);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	class URPGLocomotionComponent* LocomotionComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	class URPGPawnMovementComponent* RPGPawnMovementComponent = nullptr;
};
