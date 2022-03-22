// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGDamageTypes.h"
#include "Abilities/RPGAbilityDataTypes.h"
#include "Components/ActorComponent.h"
#include "RPGDamageHandlerComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(ActionRPG), meta=(BlueprintSpawnableComponent))
class ACTIONRPG_API URPGDamageHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGDamageHandlerComponent();

protected: // UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	void OnHealthAttributeChanged(const FOnAttributeChangeData& attributeData);

	UFUNCTION()
	virtual void OnRep_IsAlive();

public:
	// This function should always be called from authority
	void OnTakeDamage(const FRPGDamageEvent& damageEvent);

	// If bAutoDestroyOnDeath is true, destroy the owner actor after ActorLifeSpanOnDeath.
	void StartDelayedDestruction();

	// Clear the destruction timer and request destruction
	void RequestDestructionNow();

	// return whether its owner is alive or not
	bool IsAlive() const;

	FRPGEntityDeathNativeDelegate& GetDeathNativeDelegate();
	FRPGHealthChangedNativeDelegate& GetHealthChangedNativeDelegate();

public:
	UFUNCTION(NetMulticast, Reliable, Category = "RPG|Damage Handler Component")
	void NetMulticast_ReceivedDamage(const FRPGDamageEvent& damageEvent);
	
	UFUNCTION(Client, Reliable, Category = "RPG|Damage Handler Component")
	void Client_DealDamage(const FRPGDamageEvent& damageEvent);
	
	UFUNCTION(BlueprintCallable, Category = "RPG|Damage Handler Component")
	virtual void DestroyEntity();

protected:
	// This will enable ActorLifeSpanOnDeath
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Damage Handler Component")
	bool bAutoDestroyOnDeath = true;
	
	// Indicates how long its owner is visible before being destroyed
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (EditCondition = "bAutoDestroyOnDeath == true"), Category = "RPG|Damage Handler Component")
	float ActorLifeSpanOnDeath = 5.f;

	// Whether the owner of this component is still alive or not
	UPROPERTY(ReplicatedUsing = "OnRep_IsAlive", BlueprintReadOnly, Category = "RPG|Damage Handler Component")
	bool bIsAlive = true;

protected:
	UPROPERTY(BlueprintAssignable, Category = "RPG|Damage Handler Component")
	FRPGEntityDeathDelegate OnEntityDeathDelegate;
	FRPGEntityDeathNativeDelegate OnEntityDeathNativeDelegate;

	UPROPERTY(BlueprintAssignable, Category = "RPG|Damage Handler Component")
	FRPGHealthChangedDelegate OnHealthChangedDelegate;
	FRPGHealthChangedNativeDelegate OnHealthChangedNativeDelegate;

private:
	TWeakObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	FTimerHandle TimerHandle_DelayedDestructionCallback;
};
