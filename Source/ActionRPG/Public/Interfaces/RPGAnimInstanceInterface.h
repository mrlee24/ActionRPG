// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RPGAnimInstanceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class URPGAnimInstanceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONRPG_API IRPGAnimInstanceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, Category = "IRPGAnimInstanceInterface")
	virtual class USkeletalMeshComponent* GetSkeletalMeshComponent() const = 0;

	UFUNCTION(BlueprintCallable, Category = "IRPGAnimInstanceInterface")
	virtual FVector GetSocketLocation(const FName& socketName) const;
};
