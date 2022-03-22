// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RPGAttributeSet.h"
#include "UObject/Object.h"
#include "RPGAttributeSet_Attack.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API URPGAttributeSet_Attack : public URPGAttributeSet
{
	GENERATED_BODY()
	
public:

	// Initialize default values
	URPGAttributeSet_Attack();

protected: 
	
	// UEncAttributeSet overrides
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& attribute, float& newValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& attribute, float& newValue) const override;

public:

	// OnRep functions
	UFUNCTION()
	void OnRep_Damage(const FGameplayAttributeData& oldDamage) const;

	UFUNCTION()
	void OnRep_OutgoingDamageMultiplier(const FGameplayAttributeData& oldOutgoingDamageMultiplier) const;

public:

	static const float MAX_DAMAGE;
	static const float MAX_OUTGOING_DAMAGE_MULTIPLIER;
	
public:

	// Current base attack power
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Damage, Category = "RPGAttributeSet_Attack")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet_Attack, Damage)

	// Current attack power multiplier
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_OutgoingDamageMultiplier, Category = "RPGAttributeSet_Attack")
	FGameplayAttributeData OutgoingDamageMultiplier;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet_Attack, OutgoingDamageMultiplier)
};
