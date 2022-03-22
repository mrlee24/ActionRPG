// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/RPGGameplayAbility.h"

#include <valarray>

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/RPGAbilityQueueComponent.h"

DEFINE_LOG_CATEGORY(RPGGameplayAbilityLog);

URPGGameplayAbility::URPGGameplayAbility()
{
	bLooselyCheckAbilityCost = false;
	bActivateOnGranted = false;
	bEnableAbilityQueue = false;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
}

void URPGGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* actorInfo, const FGameplayAbilitySpec& spec)
{
	Super::OnAvatarSet(actorInfo, spec);

	if (actorInfo && actorInfo->AvatarActor.IsValid())
	{
		OwnerPawn = Cast<APawn>(actorInfo->AvatarActor);
	}

	if (bActivateOnGranted && actorInfo)
	{
		actorInfo->AbilitySystemComponent->TryActivateAbility(spec.Handle, false);
	}
}

bool URPGGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle handle, const FGameplayAbilityActorInfo* actorInfo,
	FGameplayTagContainer* optionalRelevantTags) const
{
	if (bLooselyCheckAbilityCost)
	{
		return true;
	}
	return Super::CheckCost(handle, actorInfo, optionalRelevantTags);
}

bool URPGGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle handle,
	const FGameplayAbilityActorInfo* actorInfo, const FGameplayTagContainer* sourceTags,
	const FGameplayTagContainer* targetTags, FGameplayTagContainer* optionalRelevantTags) const
{
	const bool bCanActivateAbility = Super::CanActivateAbility(
		handle,
		actorInfo,
		sourceTags,
		targetTags,
		optionalRelevantTags);
	
	if (!bCanActivateAbility)
	{
		return false;
	}

	if (!bHasBlueprintCanUse && bLooselyCheckAbilityCost)
	{
		return CheckForPositiveCost(handle, actorInfo, optionalRelevantTags);
	}

	return false;
}

void URPGGameplayAbility::AbilityEnded(UGameplayAbility* ability)
{
	OnAbilityEnded.Broadcast();
	OnAbilityEnded.Clear();
}

void URPGGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle handle,
                                      const FGameplayAbilityActorInfo* actorInfo, const FGameplayAbilityActivationInfo activationInfo,
                                      FOnGameplayAbilityEnded::FDelegate* onGameplayAbilityEndedDelegate, const FGameplayEventData* triggerEventData)
{
	Super::PreActivate(handle, actorInfo, activationInfo, onGameplayAbilityEndedDelegate, triggerEventData);
	OnGameplayAbilityEnded.AddUObject(this, &URPGGameplayAbility::AbilityEnded);

	// Open ability queue only if told to do so
	if (!bEnableAbilityQueue)
	{
		return;
	}

	AActor* avatarActor = GetAvatarActorFromActorInfo();
	if (!avatarActor)
	{
		return;
	}

	URPGAbilityQueueComponent* abilityQueueComponent = nullptr;
	if (!abilityQueueComponent)
	{
		return;
	}

	UE_LOG(RPGGameplayAbilityLog, Log, TEXT("%s, Open Ability Queue"), *GetName());
	abilityQueueComponent->OpenAbilityQueue();
	abilityQueueComponent->SetAllowAllAbilitiesForAbilityQueue(true);
}

bool URPGGameplayAbility::CheckForPositiveCost(const FGameplayAbilitySpecHandle handle,
	const FGameplayAbilityActorInfo* actorInfo, FGameplayTagContainer* optionalRelevantTags) const
{
	const UGameplayEffect* costGE = GetCostGameplayEffect();
	if (!costGE)
	{
		return true;
	}

	if (!CanApplyPositiveAttributeModifiers(costGE, actorInfo, GetAbilityLevel(handle, actorInfo), MakeEffectContext(handle, actorInfo)))
	{
		const FGameplayTag& costTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;
		if (optionalRelevantTags && costTag.IsValid())
		{
			optionalRelevantTags->AddTag(costTag);
		}

		return false;
	}

	return true;
}

bool URPGGameplayAbility::CanApplyPositiveAttributeModifiers(const UGameplayEffect* gameplayEffect,
	const FGameplayAbilityActorInfo* actorInfo, float level, const FGameplayEffectContextHandle& effectContext) const
{
	FGameplayEffectSpec spec(gameplayEffect, effectContext, level);
	spec.CalculateModifierMagnitudes();

	UAbilitySystemComponent* abilitySystemComponent = actorInfo->AbilitySystemComponent.IsValid() ?
		actorInfo->AbilitySystemComponent.Get() : nullptr;

	if (!abilitySystemComponent)
	{
		return false;
	}

	for (int32 modIdx = 0; modIdx < spec.Modifiers.Num(); ++modIdx)
	{
		const FGameplayModifierInfo& modDef = spec.Def->Modifiers[modIdx];
		
		// It only makes sense to check additive operators
		if (modDef.ModifierOp == EGameplayModOp::Additive)
		{
			if (!modDef.Attribute.IsValid())
			{
				continue;
			}

			if (!abilitySystemComponent->HasAttributeSetForAttribute(modDef.Attribute))
			{
				continue;
			}

			const UAttributeSet* Set = GetAttributeSubobjectForASC(abilitySystemComponent, modDef.Attribute.GetAttributeSetClass());
			const float CurrentValue = modDef.Attribute.GetNumericValueChecked(Set);

			if (CurrentValue <= 0.f)
			{
				return false;
			}
		}
	}

	return true;
}

const UAttributeSet* URPGGameplayAbility::GetAttributeSubobjectForASC(UAbilitySystemComponent* abilitySystemComponent,
	TSubclassOf<UAttributeSet> attributeClass)
{
	check(abilitySystemComponent != nullptr);

	for (const UAttributeSet* attributeSet : abilitySystemComponent->GetSpawnedAttributes())
	{
		if (attributeSet && attributeSet->IsA(attributeClass))
		{
			return attributeSet;
		}
	}

	return nullptr;
}

APawn* URPGGameplayAbility::GetOwnerPawn() const
{
	return OwnerPawn;
}
