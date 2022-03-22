// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/RPGDamageExecutionCalculation.h"

#include "Attributes/RPGAttributeSet_Health.h"
#include "Attributes/RPGAttributeSet_Attack.h"

// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct FRPGDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageApplied);
	DECLARE_ATTRIBUTE_CAPTUREDEF(OutgoingDamageMultiplier);

	FRPGDamageStatics()
	{
		// Snapshot happens at time of GESpec creation

		// We're not capturing anything from the Source in this example, but there could be like AttackPower attributes that you might want.

		// Capture the DamageApplied attribute on the target's set as that will be the one which we are modifying
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet_Health, DamageApplied, Target, false);
		
		// Capture optional Damage set on the damage GE as a CalculationModifier under the ExecutionCalculation
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet_Attack, Damage, Source, true);
		
		// Capture the Source's OutgoingDamageMultiplier.
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet_Attack, OutgoingDamageMultiplier, Source, true);
	}
};

static const FRPGDamageStatics& DamageStatics()
{
	static FRPGDamageStatics DStatics;
	return DStatics;
}

URPGDamageExecutionCalculation::URPGDamageExecutionCalculation()
{	
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageAppliedDef);
	RelevantAttributesToCapture.Add(DamageStatics().OutgoingDamageMultiplierDef);
}

void URPGDamageExecutionCalculation::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& executionParams,
	FGameplayEffectCustomExecutionOutput& outExecutionOutput) const
{
	UAbilitySystemComponent* targetAbilitySystemComponent = executionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* sourceAbilitySystemComponent = executionParams.GetSourceAbilitySystemComponent();

	const FGameplayEffectSpec& effectSpec = executionParams.GetOwningSpec();
	FGameplayTagContainer assetTags;
	effectSpec.GetAllAssetTags(assetTags);

	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* sourceTags = effectSpec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* targetTags = effectSpec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters evaluationParameters;
	evaluationParameters.SourceTags = sourceTags;
	evaluationParameters.TargetTags = targetTags;

	float outgoingDamageMultiplier = 0.0f;
	executionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().OutgoingDamageMultiplierDef,
															   evaluationParameters, outgoingDamageMultiplier);
	outgoingDamageMultiplier = FMath::Max<float>(outgoingDamageMultiplier, 0.0f);

	float damage = 0.0f;
	// Capture optional damage value set on the damage GE as a CalculationModifier under the ExecutionCalculation
	executionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, evaluationParameters, damage);
	// Add SetByCaller damage if it exists
	damage += FMath::Max<float>(
		effectSpec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, -1.0f), 0.0f);

	const float unmitigatedDamage = damage * outgoingDamageMultiplier; // Can multiply any damage boosters here
	
	// For now we don't implement any kind of formula is just a flat damage, but we will soon need it.
	const float mitigatedDamage = unmitigatedDamage;

	if (mitigatedDamage > 0.f)
	{
		// Set the Target's damage applied meta attribute
		outExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(DamageStatics().DamageAppliedProperty, EGameplayModOp::Additive,
										   mitigatedDamage));
	}
}
