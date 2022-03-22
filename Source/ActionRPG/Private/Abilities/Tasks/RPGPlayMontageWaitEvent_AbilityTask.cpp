// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Tasks/RPGPlayMontageWaitEvent_AbilityTask.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY(RPGPlayMontageWaitEvent_AbilityTaskLog);

URPGPlayMontageWaitEvent_AbilityTask::URPGPlayMontageWaitEvent_AbilityTask(const FObjectInitializer& objectInitializer)
{
	Rate = 1.f;
	bStopWhenTaskEnds = false;
	bStopWhenAbilityEnds = true;
	StartSection = NAME_None;
	StartTimeSeconds = 0.f;
}

void URPGPlayMontageWaitEvent_AbilityTask::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	bool bPlayedMontage = false;
	if (AbilitySystemComponent)
	{
		const FGameplayAbilityActorInfo* abilityActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* animInstance = abilityActorInfo->GetAnimInstance();
		if (animInstance != nullptr)
		{
			// Bind to event callback
			EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(
				EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
					this, &ThisClass::OnGameplayEvent));

			if (AbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay,
				Rate, StartSection, StartTimeSeconds) > 0.f)
			{
				// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
				if (ShouldBroadcastAbilityTaskDelegates() == false)
				{
					return;
				}

				CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(
					this, &ThisClass::OnAbilityCancelled);

				BlendingOutDelegate.BindUObject(this, &ThisClass::OnMontageBlendingOut);
				animInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
				animInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				ACharacter* character = Cast<ACharacter>(GetAvatarActor());
				if (character && (character->GetLocalRole() == ROLE_Authority ||
					(character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() ==
						EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}

				bPlayedMontage = true;
			}
		}
		else
		{
			UE_LOG(RPGPlayMontageWaitEvent_AbilityTaskLog, Warning, TEXT("Called to PlayMontage failed!"));
		}
	}
	else
	{
		UE_LOG(RPGPlayMontageWaitEvent_AbilityTaskLog, Warning,
			TEXT("Called on invalid AbilitySystemComponent"));
	}

	if (!bPlayedMontage)
	{
		UE_LOG(LogTemp, Warning,
			TEXT(
				"Called in Ability %s failed to play montage %s; Task Instance Name %s."
			), *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			//ABILITY_LOG(Display, TEXT("%s: OnCancelled"), *GetName());
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	SetWaitingOnAvatar();
}

void URPGPlayMontageWaitEvent_AbilityTask::ExternalCancel()
{
	ensure(AbilitySystemComponent);

	OnAbilityCancelled();
	Super::ExternalCancel();
}

FString URPGPlayMontageWaitEvent_AbilityTask::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;
	if (Ability)
	{
		const FGameplayAbilityActorInfo* abilityActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* animInstance = abilityActorInfo->GetAnimInstance();

		if (animInstance != nullptr)
		{
			PlayingMontage = animInstance->Montage_IsActive(MontageToPlay)
				? MontageToPlay
				: animInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(
		TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay),
		*GetNameSafe(PlayingMontage));
}

void URPGPlayMontageWaitEvent_AbilityTask::OnDestroy(bool abilityEnded)
{
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (bStopWhenTaskEnds || (abilityEnded && bStopWhenAbilityEnds))
		{
			StopPlayingMontage();
		}
	}

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}

	Super::OnDestroy(abilityEnded);
}

URPGPlayMontageWaitEvent_AbilityTask* URPGPlayMontageWaitEvent_AbilityTask::PlayMontageAndWaitForEvent(
	UGameplayAbility* owningAbility, FName taskInstanceName, UAnimMontage* montageToPlay,
	FGameplayTagContainer eventTags, float rate, FName startSection, float startTimeSeconds, bool bStopWhenTaskEnds,
	bool bStopWhenAbilityEnds, float animRootMotionTranslationScale)
{
	URPGPlayMontageWaitEvent_AbilityTask* myObj = NewAbilityTask<URPGPlayMontageWaitEvent_AbilityTask>(
		owningAbility, taskInstanceName);
	myObj->MontageToPlay = montageToPlay;
	myObj->EventTags = eventTags;
	myObj->Rate = rate;
	myObj->StartSection = startSection;
	myObj->StartTimeSeconds = startTimeSeconds;
	myObj->AnimRootMotionTranslationScale = animRootMotionTranslationScale;
	myObj->bStopWhenTaskEnds = bStopWhenTaskEnds;
	myObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

	return myObj;
}

bool URPGPlayMontageWaitEvent_AbilityTask::StopPlayingMontage() const
{
	const FGameplayAbilityActorInfo* abilityActorInfo = Ability->GetCurrentActorInfo();
	if (!abilityActorInfo)
	{
		return false;
	}

	UAnimInstance* animInstance = abilityActorInfo->GetAnimInstance();
	if (animInstance == nullptr)
	{
		return false;
	}

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	if (AbilitySystemComponent && Ability)
	{
		if (AbilitySystemComponent->GetAnimatingAbility() == Ability
			&& AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
		{
			// Unbind delegates so they don't get called as well
			FAnimMontageInstance* montageInstance = animInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (montageInstance)
			{
				montageInstance->OnMontageBlendingOutStarted.Unbind();
				montageInstance->OnMontageEnded.Unbind();
			}

			AbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}

	return false;
}

void URPGPlayMontageWaitEvent_AbilityTask::OnMontageBlendingOut(UAnimMontage* montage, bool bInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		if (montage == MontageToPlay)
		{
			AbilitySystemComponent->ClearAnimatingAbility(Ability);

			// Reset AnimRootMotionTranslationScale
			ACharacter* character = Cast<ACharacter>(GetAvatarActor());
			if (character && (character->GetLocalRole() == ROLE_Authority ||
				(character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() ==
					EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
			{
				character->SetAnimRootMotionTranslationScale(1.f);
			}
		}
	}

	if (bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void URPGPlayMontageWaitEvent_AbilityTask::OnAbilityCancelled()
{
	if (StopPlayingMontage())
	{
		// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void URPGPlayMontageWaitEvent_AbilityTask::OnMontageEnded(UAnimMontage* montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	EndTask();
}

void URPGPlayMontageWaitEvent_AbilityTask::OnGameplayEvent(FGameplayTag eventTag, const FGameplayEventData* payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData eventData = *payload;
		eventData.EventTag = eventTag;

		EventReceived.Broadcast(eventTag, MoveTemp(eventData));
	}
}