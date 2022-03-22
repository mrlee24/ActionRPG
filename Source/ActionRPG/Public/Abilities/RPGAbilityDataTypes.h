// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "UObject/Object.h"
#include "RPGAbilityDataTypes.generated.h"


USTRUCT()
struct ACTIONRPG_API FRPGGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_USTRUCT_BODY()

public:
	virtual const FGameplayAbilityTargetDataHandle& GetTargetData() const;
	virtual void AddTargetData(const FGameplayAbilityTargetDataHandle& targetDataHandle);

	//~ FGameplayEffectContext overrides
	virtual UScriptStruct* GetScriptStruct() const override;
	virtual FRPGGameplayEffectContext* Duplicate() const override;
	virtual bool NetSerialize(FArchive& ar, class UPackageMap* map, bool& bOutSuccess) override;
	//~ FGameplayEffectContext overrides

protected:
	FGameplayAbilityTargetDataHandle TargetData;
};

template<>
struct TStructOpsTypeTraits<FRPGGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FRPGGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};