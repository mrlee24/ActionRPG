// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "RPGAbilityQueueComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(RPGAbilityQueueComponentLog, Log, All);

UCLASS(ClassGroup=(ActionRPG), meta=(BlueprintSpawnableComponent))
class ACTIONRPG_API URPGAbilityQueueComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGAbilityQueueComponent();

protected: // UActorComponent interface
	virtual void BeginPlay() override;

public:
	/** Setup GetOwner to character and sets references for ability system component and the owner itself. */
	void SetupOwner();

	/**
	 * Set the bAbilityQueue to true and opens the ability queue system for activation
	 */
	void OpenAbilityQueue();

	/**
	 * Set the bAbilityQueue to false which prevents the ability queue system to activate
	 */
	void CloseAbilityQueue();

	/**
	 * Updates the Allowed Abilities for the ability queue system
	 */
	void UpdateAllowedAbilitiesForAbilityQueue(const TArray<TSubclassOf<class UGameplayAbility>>& allowedAbilities);

	/**
	 * Updates the bQueueAllowAllAbilities which prevents the check for queued abilities to be within the QueuedAllowedAbilities array
	 */
	void SetAllowAllAbilitiesForAbilityQueue(const bool bAllowAllAbilities);

	bool IsAbilityQueueOpened() const;

	bool IsAllAbilitiesAllowedForAbilityQueue() const;

	const class UGameplayAbility* GetCurrentQueuedAbility() const;

	TArray<TSubclassOf<UGameplayAbility>> GetQueuedAllowedAbilities() const;

	/**
	* Called when an ability is ended for the owner actor.
	*
	* The native implementation handles the ability queuing system, and invoke related BP event.
	*/
	void OnAbilityEnded(const class UGameplayAbility* ability);

	/**
	* Called when an ability failed to activated for the owner actor, passes along the failed ability
	* and a tag explaining why.
	*
	* The native implementation handles the ability queuing system, and invoke related BP event.
	*/
	void OnAbilityFailed(const UGameplayAbility* ability, const FGameplayTagContainer& reasonTags);

protected:
	/**
	* Reset all variables involved in the Ability Queue System to their original default values.
	*/
	virtual void ResetAbilityQueueState();
	
protected:
	// Pawn owns this component
	UPROPERTY(Transient, DuplicateTransient, BlueprintReadOnly, Category = "RPG|Ability Queue Component")
	class APawn* OwnerPawn = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Ability Queue Component")
	class UAbilitySystemComponent* OwnerAbilitySystemComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RPG|Ability Queue Component")
	uint8 bAbilityQueueEnabled : 1;

	UPROPERTY(Transient)
	const class UGameplayAbility* QueuedAbility = nullptr;

protected:
	uint8 bAbilityQueueOpened : 1;
	uint8 bAllowAllAbilitiesForAbilityQueue : 1;
	
	TArray<TSubclassOf<class UGameplayAbility>> QueuedAllowedAbilities;
};
