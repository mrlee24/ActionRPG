// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"
#include "RPGAbilitySystemComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(RPGAbilitySystemComponentLog, Log, All);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONRPG_API URPGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGAbilitySystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	/** Returns whether one of the actors's active abilities are matching the provided Ability Class */
	UFUNCTION(BlueprintPure, Category="RPG|Ability System Component")
	bool IsUsingAbilityByClass(const TSubclassOf<UGameplayAbility> abilityClass) const;
	
	/**
	* Returns a list of currently active ability instances that match the given class
	*
	* @param AbilityToSearch The Gameplay Ability Class to search for
	*/
	UFUNCTION(BlueprintCallable, Category="RPG|Ability System Component")
	TArray<class UGameplayAbility*> GetActiveAbilitiesByClass(const TSubclassOf<UGameplayAbility>& abilityToSearch) const;

	/**
	* Returns a list of currently active ability instances that match the given tags
	*
	* This only returns if the ability is currently running
	*
	* @param GameplayTagContainer The Ability Tags to search for
	*/
	UFUNCTION(BlueprintCallable, Category="RPG|Ability System Component")
	virtual TArray<class UGameplayAbility*> GetActiveAbilitiesByTags(const FGameplayTagContainer& gameplayTagContainer) const;

	/**
	* Attempts to activate the ability that is passed in. This will check costs and requirements before doing so.
	*
	* Returns true if it thinks it activated, but it may return false positives due to failure later in activation.
	*
	* @param AbilityClass Gameplay Ability Class to activate
	* @param ActivatedAbility The Gameplay Ability that was triggered on success (only returned if it is a GSCGameplayAbility)
	* @param bAllowRemoteActivation If true, it will remotely activate local/server abilities, if false it will only try to locally activate abilities.
	* @return bSuccess Returns true if it thinks it activated, but it may return false positives due to failure later in activation.
	*/
	UFUNCTION(BlueprintCallable, Category="RPG|Ability System Component")
	virtual void ActivateAbilityByClass(const TSubclassOf<UGameplayAbility> abilityClass, class UGameplayAbility*& activatedAbility,
		const bool bAllowRemoteActivation = true, bool bSuccess = false);
};

