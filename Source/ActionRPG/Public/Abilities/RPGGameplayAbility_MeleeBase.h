// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGGameplayAbility.h"
#include "Components/RPGComboManagerComponent.h"
#include "UObject/Object.h"
#include "RPGGameplayAbility_MeleeBase.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API URPGGameplayAbility_MeleeBase : public URPGGameplayAbility
{
	GENERATED_BODY()

public:
	URPGGameplayAbility_MeleeBase();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle handle, const FGameplayAbilityActorInfo* actorInfo,
		const FGameplayAbilityActivationInfo activationInfo, const FGameplayEventData* triggerEventData) override;

	UFUNCTION()
	void OnMontageCancelled(const FGameplayTag& eventTag, const FGameplayEventData& eventData);

	UFUNCTION()
	void OnMontageCompleted(const FGameplayTag& eventTag, const FGameplayEventData& eventData);

	UFUNCTION()
	void OnEventReceived(const FGameplayTag& eventTag, const FGameplayEventData& eventData);

	UFUNCTION(BlueprintPure, Category="RPG|Gameplay Ability - Melee")
	class UAnimMontage* GetNextComboMontage();
	
protected:
	UPROPERTY(BlueprintReadOnly, Transient, DuplicateTransient, Category = "RPG|Gameplay Ability - Melee")
	class URPGComboManagerComponent* ComboManagerComponent = nullptr;

	/** List of animation montages you want to cycle through when activating this ability */
	UPROPERTY(EditDefaultsOnly, Category="Montages")
	TArray<class UAnimMontage*> Montages;

	/** Change to play the montage faster or slower */
	UPROPERTY(EditDefaultsOnly, Category="Montages")
	float Rate = 1.f;

	/** Any gameplay events matching this tag will activate the OnEventReceived callback and apply the gameplay effect containers for this ability */
	UPROPERTY(EditDefaultsOnly, Category="Montages")
	FGameplayTagContainer WaitForEventTag;
};
