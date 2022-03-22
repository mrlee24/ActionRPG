// Fill out your copyright notice in the Description page of Project Settings.


#include "Attributes/RPGAttributeSet_Attack.h"

#include "Net/UnrealNetwork.h"

const float URPGAttributeSet_Attack::MAX_DAMAGE = 1000.0f;
const float URPGAttributeSet_Attack::MAX_OUTGOING_DAMAGE_MULTIPLIER = 1000.0f;

URPGAttributeSet_Attack::URPGAttributeSet_Attack()
{
	InitDamage(0.f);
	InitOutgoingDamageMultiplier(1.f);
}

void URPGAttributeSet_Attack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet_Attack, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet_Attack, OutgoingDamageMultiplier, COND_None, REPNOTIFY_Always);
}

void URPGAttributeSet_Attack::PreAttributeChange(const FGameplayAttribute& attribute, float& newValue)
{
	if (attribute == GetDamageAttribute())
	{
		newValue = FMath::Clamp(newValue, 0.0f, MAX_DAMAGE);
	}

	if (attribute == GetOutgoingDamageMultiplierAttribute())
	{
		newValue = FMath::Clamp(newValue, 0.0f, MAX_OUTGOING_DAMAGE_MULTIPLIER);
	}
}

void URPGAttributeSet_Attack::PreAttributeBaseChange(const FGameplayAttribute& attribute, float& newValue) const
{
	if (attribute == GetDamageAttribute())
	{
		newValue = FMath::Clamp(newValue, 0.0f, MAX_DAMAGE);
	}

	if (attribute == GetOutgoingDamageMultiplierAttribute())
	{
		newValue = FMath::Clamp(newValue, 0.0f, MAX_OUTGOING_DAMAGE_MULTIPLIER);
	}
}

void URPGAttributeSet_Attack::OnRep_Damage(const FGameplayAttributeData& oldDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet_Attack, Damage, oldDamage);
}

void URPGAttributeSet_Attack::OnRep_OutgoingDamageMultiplier(
	const FGameplayAttributeData& oldOutgoingDamageMultiplier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet_Attack, OutgoingDamageMultiplier, oldOutgoingDamageMultiplier);
}
