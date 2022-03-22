// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/RPGGameplayEffect.h"

#include "AbilitySystemComponent.h"

void URPGGameplayEffect::ApplyDynamicGameplayEffect(const FRPGDynamicGameplayEffectData& data)
{
	// Kick out of the call if data's AbilitySystemComponent is invalid.
	if (!ensureAlwaysMsgf(data.AbilitySystemComponent.IsValid(),
		TEXT("%s is called but the passed FRPGDynamicGameplayEffectData's AbilitySystemComponent is null."), ANSI_TO_TCHAR(__func__)))
	{
		return;
	}

	// Dynamically create the gameplay effect.
	URPGGameplayEffect* ge = NewObject<URPGGameplayEffect>(GetTransientPackage(), data.EffectName);
	// Dynamic gameplay effects must have an instant duration type.
	ge->DurationPolicy = EGameplayEffectDurationType::Instant;

	// Add one new modifier to the gameplay effect.
	const int32 modifierIndex = ge->Modifiers.Num();
	ge->Modifiers.SetNum(modifierIndex + 1);

	// Grab the modifier at the appropriate index and set its data appropriately.
	FGameplayModifierInfo& modInfo = ge->Modifiers[modifierIndex];
	modInfo.ModifierMagnitude = data.EffectMagnitude;
	modInfo.ModifierOp = data.EffectModifierOpType;
	modInfo.Attribute = data.AffectedAttribute;

	// Apply the gameplay effect to the TowersCharacter's ability system component.
	data.AbilitySystemComponent->ApplyGameplayEffectToSelf(ge, data.EffectLevel, data.AbilitySystemComponent->MakeEffectContext());
}
