// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "UObject/Object.h"
#include "RPGGameplayAbility.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(RPGGameplayAbilityLog, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRPGAbilityEnded);
/**
 * 
 */
UCLASS()
class ACTIONRPG_API URPGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URPGGameplayAbility();

public: // UGameplayAbility interface
	// To handle "passive" abilities, activating abilities marked as "Activate On Granted" automatically 
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* actorInfo, const FGameplayAbilitySpec& spec) override;

	/**
	 * Checks cost. returns true if we can pay for the ability. False if not
	 *
	 * If the Ability is set to ignore ability cost via bIgnoreAbilityCost, returns true
	 */
	virtual bool CheckCost(const FGameplayAbilitySpecHandle handle, const FGameplayAbilityActorInfo* actorInfo, FGameplayTagContainer* optionalRelevantTags) const override;

	/**
	 * Returns true if this ability can be activated right now. Has no side effects
	 *
	 * This optionally loose Cost check if the ability is marked as ignore cost,
	 * meaning cost attributes are only checked to be < 0 and prevented if 0 or below.
	 *
	 * If Blueprints implements the CanActivateAbility function, they are responsible for ability activation or not
	 */
	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle handle, 
		const FGameplayAbilityActorInfo* actorInfo, 
		const FGameplayTagContainer* sourceTags, 
		const FGameplayTagContainer* targetTags, 
		FGameplayTagContainer* optionalRelevantTags) const override;

	void AbilityEnded(class UGameplayAbility* ability);

protected: // UGameplayAbility interface

	// Overrides Ability pre-activate, namely to open queue system if the ability is flagged as is
	virtual void PreActivate(
		const FGameplayAbilitySpecHandle handle, 
		const FGameplayAbilityActorInfo* actorInfo, 
		const FGameplayAbilityActivationInfo activationInfo, 
		FOnGameplayAbilityEnded::FDelegate* onGameplayAbilityEndedDelegate, 
		const FGameplayEventData* triggerEventData) override;

private:
	/** Loosely Check for cost attribute current value to be positive */
	bool CheckForPositiveCost(const FGameplayAbilitySpecHandle handle, const FGameplayAbilityActorInfo* actorInfo, OUT FGameplayTagContainer* optionalRelevantTags) const;

	// Does the actual check for attribute modifiers, only checking it their current value is <= 0
	bool CanApplyPositiveAttributeModifiers(const UGameplayEffect* gameplayEffect, const FGameplayAbilityActorInfo* actorInfo, float level, const FGameplayEffectContextHandle& effectContext) const;

	// Returns spawned attribute set from passed in ASC based on provided AttributeClass (mainly because GetAttributeSuboject on AbilitySystemComponent is protected)
	static const UAttributeSet* GetAttributeSubobjectForASC(UAbilitySystemComponent* abilitySystemComponent, TSubclassOf<UAttributeSet> attributeClass);

public:
	// Return the owner of this ability 
	class APawn* GetOwnerPawn() const;

public:
	/**
	 * Called when the ability ends.
	 */
	UPROPERTY(BlueprintAssignable, Category = "RPG|Ability")
	FRPGAbilityEnded OnAbilityEnded;
	
	/** if true, the ability can be activated even it the cost is going into negative values, preventing activation only if cost is below or equal to 0 already */
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	uint8 bLooselyCheckAbilityCost : 1;

	/**
	 * If true, the ability will be automatically activated as soon as it is granted.
	 *
	 * You can either implement one-off Abilities that are meant to be activated right away when granted,
	 * or "Passive Abilities" with this, an ability that automatically activates and run continuously (eg. not calling EndAbility)
	 *
	 * In both case, GameplayAbilities configured to be activated on granted will only activate on server and typically have
	 * a Net Execution Policy of Server Only.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	uint8 bActivateOnGranted : 1;

	/**
	 * Enable other abilities to be queued and activated when this ability ends.
	 *
	 * It is recommended to leave this variable to false, and instead use the AbilityQueueNotifyState (AbilityQueueWindow)
	 * within montages to further tune when the Queue System is opened and closed.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	uint8 bEnableAbilityQueue : 1;

protected:
	UPROPERTY(Transient, DuplicateTransient)
	class APawn* OwnerPawn = nullptr;
};
