// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGComboManagerComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/RPGAbilitySystemComponent.h"
#include "Abilities/RPGGameplayAbility_MeleeBase.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(RPGComboManagerComponentLog);

// Sets default values for this component's properties
URPGComboManagerComponent::URPGComboManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...

	MeleeBaseAbility = URPGGameplayAbility_MeleeBase::StaticClass();
}


// Called when the game starts
void URPGComboManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Setup Owner references
	SetupOwner();

	// Cache net role here as well since for map-placed actors on clients, the Role may not be set correctly yet in OnRegister.
	CacheIsNetSimulated();
}

void URPGComboManagerComponent::OnRegister()
{
	Super::OnRegister();

	// Cached off netrole to avoid constant checking on owning actor
	CacheIsNetSimulated();
}

void URPGComboManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ComboIndex, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bComboWindowOpened, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bShouldTriggerCombo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bRequestTriggerCombo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bNextComboAbilityActivated, COND_None, REPNOTIFY_Always);
}

void URPGComboManagerComponent::SetupOwner()
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

	OwnerAbilitySystemComponent = Cast<URPGAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPawn));
}

UGameplayAbility* URPGComboManagerComponent::GetCurrentActiveComboAbility() const
{
	if (OwnerAbilitySystemComponent)
	{
		const TArray<UGameplayAbility*> abilities = OwnerAbilitySystemComponent->GetActiveAbilitiesByClass(MeleeBaseAbility);
		return abilities.IsValidIndex(0) ? abilities[0] : nullptr;
	}

    UE_LOG(RPGComboManagerComponentLog, Error, TEXT("OwnerAbilitySystemComponent is invalid return nothing"));
	return nullptr;
}

void URPGComboManagerComponent::SetComboIndex(const int32 comboIndex)
{
	ComboIndex = comboIndex;
	if (!IsOwnerActorAuthoritative())
	{
		Server_SetComboIndex(comboIndex);
	}
}

bool URPGComboManagerComponent::IsOwnerActorAuthoritative() const
{
	return !bCachedIsNetSimulated;
}

void URPGComboManagerComponent::IncrementCombo()
{
	if (bComboWindowOpened)
	{
		ComboIndex += 1;
	}
}

void URPGComboManagerComponent::ResetCombo()
{
	UE_LOG(RPGComboManagerComponentLog, Verbose, TEXT("ResetCombo() is called"));
	SetComboIndex(0);
}

void URPGComboManagerComponent::ActivateComboAbility(TSubclassOf<URPGGameplayAbility_MeleeBase> abilityClass,
	bool bAllowRemoteActivation)
{
	(IsOwnerActorAuthoritative()) ?
		Internal_ActivateComboAbility(abilityClass, bAllowRemoteActivation) :
		Server_ActivateComboAbility(abilityClass, bAllowRemoteActivation);
}

void URPGComboManagerComponent::CacheIsNetSimulated()
{
	bCachedIsNetSimulated = IsNetSimulating();
}

void URPGComboManagerComponent::Server_ActivateComboAbility_Implementation(
	TSubclassOf<URPGGameplayAbility_MeleeBase> meleeAbilityClass, bool bAllowRemoteActivation)
{
	NetMulticast_ActivateComboAbility(meleeAbilityClass, bAllowRemoteActivation);
}

void URPGComboManagerComponent::NetMulticast_ActivateComboAbility_Implementation(
	TSubclassOf<URPGGameplayAbility_MeleeBase> meleeAbilityClass, bool bAllowRemoteActivation)
{
	if (OwnerPawn && !OwnerPawn->IsLocallyControlled())
	{
		Internal_ActivateComboAbility(meleeAbilityClass, bAllowRemoteActivation);
	}
}

void URPGComboManagerComponent::Internal_ActivateComboAbility(
	TSubclassOf<URPGGameplayAbility_MeleeBase> meleeAbilityClass, bool bAllowRemoteActivation)
{
	bShouldTriggerCombo = false;
	if (!OwnerPawn)
	{
		UE_LOG(RPGComboManagerComponentLog, Error, TEXT("OwnerPawn is nullptr"));
		return;
	}

	if (!meleeAbilityClass)
	{
		UE_LOG(RPGComboManagerComponentLog, Error, TEXT("Provided AbilityClass is nullptr"));
		return;
	}

	if (OwnerAbilitySystemComponent->IsUsingAbilityByClass(meleeAbilityClass))
	{
		UE_LOG(
			RPGComboManagerComponentLog,
			Verbose,
			TEXT("%s is using %s already, update should trigger combo to %s"),
			*GetName(),
			*meleeAbilityClass->GetName(),
			bComboWindowOpened ? TEXT("true") : TEXT("false"));

		bShouldTriggerCombo = bComboWindowOpened;
	}
	else
	{
		UE_LOG(
			RPGComboManagerComponentLog,
			Verbose,
			TEXT("%s is not in combo, activate %s"),
			*GetName(), *meleeAbilityClass->GetName());
		UGameplayAbility* tempActivateAbility = nullptr;
		OwnerAbilitySystemComponent->ActivateAbilityByClass(meleeAbilityClass, tempActivateAbility, bAllowRemoteActivation);
	}
}

void URPGComboManagerComponent::Server_SetComboIndex_Implementation(const int32 comboIndex)
{
	NetMulticast_SetComboIndex(comboIndex);
}

void URPGComboManagerComponent::NetMulticast_SetComboIndex_Implementation(const int32 comboIndex)
{
	if (OwnerPawn && !OwnerPawn->IsLocallyControlled())
	{
		ComboIndex = comboIndex;
	}
}

