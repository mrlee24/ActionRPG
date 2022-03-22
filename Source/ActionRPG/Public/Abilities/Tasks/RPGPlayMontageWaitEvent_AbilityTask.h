// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "RPGPlayMontageWaitEvent_AbilityTask.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(RPGPlayMontageWaitEvent_AbilityTaskLog, Log, All);

// Delegate type used, EventTag and Payload may be empty if it came from the montage callbacks 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGPlayMontageAndWaitForEventDelegate, const FGameplayTag&, eventTag, const FGameplayEventData&, eventData);

/**
* This task combines PlayMontageAndWait and WaitForEvent into one task, so you can wait for multiple types of activations such as from a melee combo
* Much of this code is copied from one of those two ability tasks
* This is a good task to look at as an example when creating game-specific tasks
* It is expected that each game will have a set of game-specific tasks to do what they want
*/
UCLASS()
class ACTIONRPG_API URPGPlayMontageWaitEvent_AbilityTask : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	// Constructor and overrides
	URPGPlayMontageWaitEvent_AbilityTask(const FObjectInitializer& objectInitializer);

	/*
	* The Blueprint node for this task, PlayMontageAndWaitForEvent, has some black magic from the plugin that automagically calls Activate()
	* inside of K2Node_LatentAbilityCall as stated in the AbilityTask.h. Ability logic written in C++ probably needs to call Activate() itself manually.
	*/
	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool abilityEnded) override;

	/**
	 * Play a montage and wait for it end. If a gameplay event happens that matches EventTags (or EventTags is empty), the EventReceived delegate will fire with a tag and event data.
	 * If StopWhenAbilityEnds is true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled.
	 * On normal execution, OnBlendOut is called when the montage is blending out, and OnCompleted when it is completely done playing
	 * OnInterrupted is called if another montage overwrites this, and OnCancelled is called if the ability or task is cancelled
	 *
	 * @param taskInstanceName Set to override the name of this task, for later querying
	 * @param montageToPlay The montage to play on the character
	 * @param eventTags Any gameplay events matching this tag will activate the EventReceived callback. If empty, all events will trigger callback
	 * @param rate Change to play the montage faster or slower
	 * @param bStopWhenTaskEnds If true, montage will end if the task ends or the ability ends, and ignore bStopWhenAbilityEnds. NOTE: if the task is already ended, bStopWhenAbilityEnds will also be ignored when ability ends!
	 * @param bStopWhenAbilityEnds If true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled. NOTE: Must NOT end this task before ending the ability!
	 * @param animRootMotionTranslationScale Change to modify size of root motion or set to 0 to block it entirely
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf =
	"OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static URPGPlayMontageWaitEvent_AbilityTask* PlayMontageAndWaitForEvent(
			UGameplayAbility* owningAbility,
			FName taskInstanceName,
			UAnimMontage* montageToPlay,
			FGameplayTagContainer eventTags,
			float rate = 1.f,
			FName startSection = NAME_None,
			float startTimeSeconds = 0.f,
			bool bStopWhenTaskEnds = false,
			bool bStopWhenAbilityEnds = true,
			float animRootMotionTranslationScale = 1.f);


	// Checks if the ability is playing a montage and stops that montage, returns true if a montage was stopped, false if not. 
	bool StopPlayingMontage() const;

	void OnMontageBlendingOut(UAnimMontage* montage, bool bInterrupted);
	void OnAbilityCancelled();
	void OnMontageEnded(UAnimMontage* montage, bool bInterrupted);
	void OnGameplayEvent(FGameplayTag eventTag, const FGameplayEventData* payload);

public:

	// The montage completely finished playing 
	UPROPERTY(BlueprintAssignable)
	FRPGPlayMontageAndWaitForEventDelegate OnCompleted;

	// The montage started blending out 
	UPROPERTY(BlueprintAssignable)
	FRPGPlayMontageAndWaitForEventDelegate OnBlendOut;

	// The montage was interrupted 
	UPROPERTY(BlueprintAssignable)
	FRPGPlayMontageAndWaitForEventDelegate OnInterrupted;

	// The ability task was explicitly cancelled by another ability 
	UPROPERTY(BlueprintAssignable)
	FRPGPlayMontageAndWaitForEventDelegate OnCancelled;

	// One of the triggering gameplay events happened 
	UPROPERTY(BlueprintAssignable)
	FRPGPlayMontageAndWaitForEventDelegate EventReceived;

private:

	// Montage that is playing 
	UPROPERTY()
	UAnimMontage* MontageToPlay;

	// List of tags to match against gameplay events 
	UPROPERTY()
	FGameplayTagContainer EventTags;

	// Playback rate 
	UPROPERTY()
	float Rate;

	// Section to start montage from 
	UPROPERTY()
	FName StartSection;

	UPROPERTY()
	float StartTimeSeconds;

	// Modifies how root motion movement to apply 
	UPROPERTY()
	float AnimRootMotionTranslationScale;

	// If true, the current montage will end if task ends or ability ends, ignore bStopWhenAbilityEnds
	UPROPERTY()
	bool bStopWhenTaskEnds;

	// Rather montage should be aborted if ability ends 
	UPROPERTY()
	bool bStopWhenAbilityEnds;

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle CancelledHandle;
	FDelegateHandle EventHandle;
};
