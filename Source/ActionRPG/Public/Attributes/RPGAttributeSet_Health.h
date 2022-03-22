// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RPGAttributeSet.h"
#include "UObject/Object.h"
#include "RPGAttributeSet_Health.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API URPGAttributeSet_Health : public URPGAttributeSet
{
	GENERATED_BODY()

public:
	URPGAttributeSet_Health();

public: // UAttributeSet Interface
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& attribute, float& newValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& attribute, float& newValue) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& data) override;

public:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& oldHealth) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& oldMaxHealth) const;

public:
	// Current Health Attribute
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "RPGAttributeSet_Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet_Health, Health);

	// Current Max Health Attribute
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "RPGAttributeSet_Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet_Health, MaxHealth);

	/*
	* Damage is a temporary Value:
	*   - Used for damage calculations only.
	*   - Exists only server side
	*   - It is NOT replicated
	* Damage will be turned into -health modifier which in turn
	* will modify the actual Health attribute from this attribute set.
	*/
	UPROPERTY(Transient, meta = (HideFromLevelInfos))
	FGameplayAttributeData DamageApplied;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet_Health, DamageApplied)
};
