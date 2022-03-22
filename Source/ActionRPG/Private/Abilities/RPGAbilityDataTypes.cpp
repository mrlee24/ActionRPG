// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/RPGAbilityDataTypes.h"

const FGameplayAbilityTargetDataHandle& FRPGGameplayEffectContext::GetTargetData() const
{
	return TargetData;
}

void FRPGGameplayEffectContext::AddTargetData(const FGameplayAbilityTargetDataHandle& targetDataHandle)
{
	TargetData.Append(targetDataHandle);
}

UScriptStruct* FRPGGameplayEffectContext::GetScriptStruct() const
{
	return FGameplayEffectContext::StaticStruct();
}

FRPGGameplayEffectContext* FRPGGameplayEffectContext::Duplicate() const
{
	FRPGGameplayEffectContext* effectContext = new FRPGGameplayEffectContext();
	*effectContext = *this;
	effectContext->AddActors(Actors);
	if (GetHitResult())
	{
		// Does a deep copy of the hit result
		effectContext->AddHitResult(*GetHitResult(), true);
	}

	// Shallow copy of TargetData
	effectContext->TargetData.Append(TargetData);
	return effectContext;
}

bool FRPGGameplayEffectContext::NetSerialize(FArchive& ar, UPackageMap* map, bool& bOutSuccess)
{
	return FGameplayEffectContext::NetSerialize(ar, map, bOutSuccess) &&
		TargetData.NetSerialize(ar, map, bOutSuccess);
}
