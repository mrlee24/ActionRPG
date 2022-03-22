// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RPGDamageTypes.h"
#include "UObject/Interface.h"
#include "RPGDamagableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class URPGDamagableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONRPG_API IRPGDamagableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual class URPGDamageHandlerComponent* GetDamageHandlerComponent() const = 0;

	// Called after damage has been applied on authority and FTowersDamageEvent has been generated.
	// Redirect the OnTakeDamage call to the Damage Handler Component
	virtual void OnTakeDamage(const FRPGDamageEvent& damageEvent);

	virtual void Client_DealDamage(const FRPGDamageEvent& damageEvent);
	
	virtual FRPGEntityDeathNativeDelegate& GetEntityDeathNativeDelegate();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IRPGDamagableInterface")
	bool IsAlive() const;
	virtual bool IsAlive_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IRPGDamagableInterface")
	void Kill(AActor* instigatorActor);
	virtual void Kill_Implementation(AActor* instigatorActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IRPGDamagableInterface")
	void PerformHitReaction(const FRPGDamageEvent& damageEvent);
	virtual void PerformHitReaction_Implementation(const FRPGDamageEvent& DamageEvent) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ILSDamagableInterface")
	ERPGDamageZone GetDamageZoneByBoneName(const FName& boneName) const;
	virtual ERPGDamageZone GetDamageZoneByBoneName_Implementation(const FName& boneName) const;
};
