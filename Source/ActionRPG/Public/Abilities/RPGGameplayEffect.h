// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "UObject/Object.h"
#include "RPGGameplayEffect.generated.h"

USTRUCT()
struct ACTIONRPG_API FRPGDynamicGameplayEffectData
{
	GENERATED_BODY()
public:
	// The magnitude or efficacy of the gameplay effect.
	FGameplayEffectModifierMagnitude EffectMagnitude = FScalableFloat(1.f);

	// The attribute to which the gameplay effect will be applied.
	FGameplayAttribute AffectedAttribute = {};

	// The gameplay effect's name.
	FName EffectName = FName(TEXT("EffectName"));

	// A pointer to the ability system component to which the gameplay effect will be applied.
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// The manner in which the gameplay effect will be applied (additive, multiplicative, etc.).
	TEnumAsByte<EGameplayModOp::Type> EffectModifierOpType = EGameplayModOp::Additive;

	// The level of the gameplay effect, which may in part determine its efficacy.
	float EffectLevel = 1.f;
};

/**
 * 
 */
UCLASS()
class ACTIONRPG_API URPGGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	/**
	* Applies a dynamic gameplay effect at runtime, and in accordance with the passed FTowersDynamicGameplayEffectData.
	* @param	data - struct containing all of the relevant data by which the dynamic gameplay effect may be applied.
	*/
	static void ApplyDynamicGameplayEffect(const FRPGDynamicGameplayEffectData& data);
};
