// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/RPGGameplayAbility_MeleeBase.h"

#include "Abilities/Tasks/RPGPlayMontageWaitEvent_AbilityTask.h"

URPGGameplayAbility_MeleeBase::URPGGameplayAbility_MeleeBase()
{
}

void URPGGameplayAbility_MeleeBase::ActivateAbility(const FGameplayAbilitySpecHandle handle,
	const FGameplayAbilityActorInfo* actorInfo, const FGameplayAbilityActivationInfo activationInfo,
	const FGameplayEventData* triggerEventData)
{
	if (!CommitAbility(handle, actorInfo, activationInfo))
	{
		EndAbility(handle, actorInfo, activationInfo, true, true);
		return;
	}

	AActor* avatarActor = GetAvatarActorFromActorInfo();
	if (!avatarActor)
	{
		EndAbility(handle, actorInfo, activationInfo, true, true);
		return;
	}

	ComboManagerComponent = nullptr;
	if (!ComboManagerComponent)
	{
		EndAbility(handle, actorInfo, activationInfo, true, true);
		return;
	}

	ComboManagerComponent->IncrementCombo();

	UAnimMontage* Montage = GetNextComboMontage();

	URPGPlayMontageWaitEvent_AbilityTask* Task = URPGPlayMontageWaitEvent_AbilityTask::PlayMontageAndWaitForEvent(this, NAME_None, Montage, WaitForEventTag,
		Rate, NAME_None, true, 1.0f);
	Task->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
	Task->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
	Task->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
	Task->EventReceived.AddDynamic(this, &ThisClass::OnEventReceived);
	Task->ReadyForActivation();
}

void URPGGameplayAbility_MeleeBase::OnMontageCancelled(const FGameplayTag& eventTag, const FGameplayEventData& eventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URPGGameplayAbility_MeleeBase::OnMontageCompleted(const FGameplayTag& eventTag, const FGameplayEventData& eventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URPGGameplayAbility_MeleeBase::OnEventReceived(const FGameplayTag& eventTag, const FGameplayEventData& eventData)
{
	// ApplyEffectContainer(EventTag, EventData);
}

UAnimMontage* URPGGameplayAbility_MeleeBase::GetNextComboMontage()
{
	if (!ComboManagerComponent)
	{
		return nullptr;
	}

	int32 ComboIndex = ComboManagerComponent->ComboIndex;

	if (ComboIndex >= Montages.Num())
	{
		ComboIndex = 0;
	}

	return Montages.IsValidIndex(ComboIndex) ? Montages[ComboIndex] : nullptr;
}
