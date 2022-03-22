// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "RPGDamageExecutionCalculation.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API URPGDamageExecutionCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	
	// Initialize default values
	URPGDamageExecutionCalculation();

public:
	
	// UGameplayEffectExecutionCalculation overrides
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& executionParams, OUT FGameplayEffectCustomExecutionOutput& outExecutionOutput) const override;
};
