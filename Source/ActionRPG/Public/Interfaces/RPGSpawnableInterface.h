// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Utils/RPGUtilsTypes.h"
#include "RPGSpawnableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class URPGSpawnableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONRPG_API IRPGSpawnableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IRPGSpawnableInterface")
	void DespawnSpawnable(const UObject* instigator, const ERPGEndPlayReason::Type endReason);
	virtual void DespawnSpawnable_Implementation(const UObject* instigator, const ERPGEndPlayReason::Type endReason);
};
