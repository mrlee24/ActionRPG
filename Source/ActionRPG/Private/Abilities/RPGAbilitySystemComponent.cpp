// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/RPGAbilitySystemComponent.h"

DEFINE_LOG_CATEGORY(RPGAbilitySystemComponentLog);

// Sets default values for this component's properties
URPGAbilitySystemComponent::URPGAbilitySystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URPGAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

bool URPGAbilitySystemComponent::IsUsingAbilityByClass(const TSubclassOf<UGameplayAbility> abilityClass) const
{
	if (!abilityClass)
	{
		UE_LOG(RPGAbilitySystemComponentLog, Error, TEXT("Provided AbilityClass is nullptr"));
		return false;
	}

	return GetActiveAbilitiesByClass(abilityClass).Num() > 0;
}

TArray<UGameplayAbility*> URPGAbilitySystemComponent::GetActiveAbilitiesByClass(
	const TSubclassOf<UGameplayAbility>& abilityToSearch) const
{
	TArray<FGameplayAbilitySpec> specs = GetActivatableAbilities();
	TArray<FGameplayAbilitySpec*> matchingGameplayAbilities;
	TArray<UGameplayAbility*> activeAbilities;

	// First, search for matching Abilities for this class
	for (const FGameplayAbilitySpec& spec : specs)
	{
		if (spec.Ability && spec.Ability->GetClass()->IsChildOf(abilityToSearch))
		{
			matchingGameplayAbilities.Add(const_cast<FGameplayAbilitySpec*>(&spec));
		}
	}

	// Iterate the list of all ability specs
	for (const FGameplayAbilitySpec* spec : matchingGameplayAbilities)
	{
		// Iterate all instances on this ability spec, which can include instance per execution abilities
		TArray<UGameplayAbility*> abilityInstances = spec->GetAbilityInstances();

		for (UGameplayAbility* activeAbility : abilityInstances)
		{
			if (activeAbility->IsActive())
			{
				activeAbilities.Add(activeAbility);
			}
		}
	}

	return activeAbilities;
}

TArray<UGameplayAbility*> URPGAbilitySystemComponent::GetActiveAbilitiesByTags(
	const FGameplayTagContainer& gameplayTagContainer) const
{
	TArray<UGameplayAbility*> activeAbilities;
	TArray<FGameplayAbilitySpec*> matchingGameplayAbilities;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(gameplayTagContainer,
		matchingGameplayAbilities, false);

	// Iterate the list of all ability specs
	for (const FGameplayAbilitySpec* spec : matchingGameplayAbilities)
	{
		// Iterate all instances on this ability spec
		TArray<UGameplayAbility*> abilityInstances = spec->GetAbilityInstances();
		for (UGameplayAbility* activeAbility : abilityInstances)
		{
			if (activeAbility->IsActive())
			{
				activeAbilities.Add(activeAbility);
			}
		}
	}

	return activeAbilities;
}

void URPGAbilitySystemComponent::ActivateAbilityByClass(const TSubclassOf<UGameplayAbility> abilityClass,
	UGameplayAbility*& activatedAbility, bool bAllowRemoteActivation, bool bSuccess)
{
	bSuccess = TryActivateAbilityByClass(abilityClass, bAllowRemoteActivation);

	const TArray<UGameplayAbility*> activeAbilities = GetActiveAbilitiesByClass(abilityClass);
	if (activeAbilities.Num() == 0)
	{
		UE_LOG(RPGAbilitySystemComponentLog, Warning,
			TEXT("Couldn't get back active abilities with Class %s"), *abilityClass->GetName());
	}

	if (bSuccess && activeAbilities.Num() > 0)
	{
		if (activeAbilities.IsValidIndex(0))
		{
			activatedAbility = activeAbilities[0];
		}
	}
}


