// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGAbilityQueueComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

DEFINE_LOG_CATEGORY(RPGAbilityQueueComponentLog);

// Sets default values for this component's properties
URPGAbilityQueueComponent::URPGAbilityQueueComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	SetIsReplicatedByDefault(true);

	bAbilityQueueEnabled = true;
	bAbilityQueueEnabled = false;
	bAllowAllAbilitiesForAbilityQueue = false;
}

// Called when the game starts
void URPGAbilityQueueComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	SetupOwner();
}

void URPGAbilityQueueComponent::SetupOwner()
{
	if (!GetOwner())
	{
		return;
	}

	OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPawn);
}

void URPGAbilityQueueComponent::OpenAbilityQueue()
{
	if (!bAbilityQueueEnabled)
	{
		return;
	}

	bAbilityQueueEnabled = false;
}

void URPGAbilityQueueComponent::CloseAbilityQueue()
{
	if (!bAbilityQueueEnabled)
	{
		return;
	}

	if (OwnerPawn)
	{
		UE_LOG(RPGAbilityQueueComponentLog, Verbose, TEXT("Closing Ability Queue for %s"), *OwnerPawn->GetName());
	}

	bAbilityQueueOpened = false;
}

void URPGAbilityQueueComponent::UpdateAllowedAbilitiesForAbilityQueue(
	const TArray<TSubclassOf<UGameplayAbility>>& allowedAbilities)
{
	if (bAbilityQueueEnabled)
	{
		return;
	}

	QueuedAllowedAbilities = allowedAbilities;
}

void URPGAbilityQueueComponent::SetAllowAllAbilitiesForAbilityQueue(const bool bAllowAllAbilities)
{
	if (!bAbilityQueueEnabled)
	{
		return;
	}

	bAllowAllAbilitiesForAbilityQueue = bAllowAllAbilities;
}

bool URPGAbilityQueueComponent::IsAbilityQueueOpened() const
{
	return bAbilityQueueOpened;
}

bool URPGAbilityQueueComponent::IsAllAbilitiesAllowedForAbilityQueue() const
{
	return bAllowAllAbilitiesForAbilityQueue;
}

const UGameplayAbility* URPGAbilityQueueComponent::GetCurrentQueuedAbility() const
{
	return QueuedAbility;
}

TArray<TSubclassOf<UGameplayAbility>> URPGAbilityQueueComponent::GetQueuedAllowedAbilities() const
{
	return QueuedAllowedAbilities;
}

void URPGAbilityQueueComponent::OnAbilityEnded(const UGameplayAbility* ability)
{
	if (bAbilityQueueEnabled)
	{
		if (QueuedAbility)
		{
			// Store queue ability in a local var, it is cleared in ResetAbilityQueueState
			const UGameplayAbility* abilityToActivate = QueuedAbility;
			UE_LOG(RPGAbilityQueueComponentLog, Log, TEXT("Has a queued input: %s [AbilityQueueSystem]"), *abilityToActivate->GetName());
			if (bAllowAllAbilitiesForAbilityQueue || QueuedAllowedAbilities.Contains(abilityToActivate->GetClass()))
			{
				ResetAbilityQueueState();
				
				UE_LOG(RPGAbilityQueueComponentLog, Log,
					TEXT("%s is already in Allowed Abilities, try to activate [AbilityQueueSystem]"), *abilityToActivate->GetName());
				if (OwnerAbilitySystemComponent)
				{
					OwnerAbilitySystemComponent->TryActivateAbilityByClass(abilityToActivate->GetClass());
				}
			}
			else
			{
				ResetAbilityQueueState();
				UE_LOG(RPGAbilityQueueComponentLog, Verbose,
					TEXT("This ability is not in allowed ability, do nothing: %s [AbilityQueueSystem]"), *abilityToActivate->GetName());
			}
		}
		else
		{
			ResetAbilityQueueState();
		}
	}
}

void URPGAbilityQueueComponent::OnAbilityFailed(const UGameplayAbility* ability,
	const FGameplayTagContainer& reasonTags)
{
	UE_LOG(RPGAbilityQueueComponentLog, Verbose, TEXT("%s, Reason: %s"), *ability->GetName(), *reasonTags.ToStringSimple());
	if (bAbilityQueueEnabled && bAbilityQueueOpened)
	{
		UE_LOG(RPGAbilityQueueComponentLog, Verbose,
			TEXT("Set QueuedAbility to %s"), *ability->GetName());
		if (bAllowAllAbilitiesForAbilityQueue || QueuedAllowedAbilities.Contains(ability->GetClass()))
		{
			QueuedAbility = ability;
		}
	}
}

void URPGAbilityQueueComponent::ResetAbilityQueueState()
{
	UE_LOG(RPGAbilityQueueComponentLog, Verbose, TEXT("ResetAbilityQueueState() is fired"));
	QueuedAbility = nullptr;
	bAllowAllAbilitiesForAbilityQueue = false;
	QueuedAllowedAbilities.Empty();
}
