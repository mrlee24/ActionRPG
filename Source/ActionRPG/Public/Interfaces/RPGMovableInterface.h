// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RPGMovableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class URPGMovableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONRPG_API IRPGMovableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, Category = "IRPGMovableInterface")
	virtual class UMovementComponent* GetBaseMovementComponent() const = 0;

	UFUNCTION(BlueprintCallable, Category = "IRPGMovableInterface")
	virtual void SetRotationYawRate(const float rotationYawRate = 0.f) const = 0;
};
