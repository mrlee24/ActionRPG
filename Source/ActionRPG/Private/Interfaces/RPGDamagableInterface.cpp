// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/RPGDamagableInterface.h"

#include "Components/RPGDamageHandlerComponent.h"

// Add default functionality here for any IRPGDamagableInterface functions that are not pure virtual.
void IRPGDamagableInterface::OnTakeDamage(const FRPGDamageEvent& damageEvent)
{
	URPGDamageHandlerComponent* damageHandlerComponent = GetDamageHandlerComponent();
	check(damageHandlerComponent);	
	return damageHandlerComponent->NetMulticast_ReceivedDamage(damageEvent);
}

void IRPGDamagableInterface::Client_DealDamage(const FRPGDamageEvent& damageEvent)
{
	URPGDamageHandlerComponent* damageHandlerComponent = GetDamageHandlerComponent();
	check(damageHandlerComponent);
	damageHandlerComponent->Client_DealDamage(damageEvent);
}

FRPGEntityDeathNativeDelegate& IRPGDamagableInterface::GetEntityDeathNativeDelegate()
{
	URPGDamageHandlerComponent* damageHandlerComponent = GetDamageHandlerComponent();
	check(damageHandlerComponent);
	return damageHandlerComponent->GetDeathNativeDelegate();
}

bool IRPGDamagableInterface::IsAlive_Implementation() const
{
	URPGDamageHandlerComponent* damageHandlerComponent = GetDamageHandlerComponent();
	return damageHandlerComponent ? damageHandlerComponent->IsAlive() : true;
}

void IRPGDamagableInterface::Kill_Implementation(AActor* instigatorActor)
{
	// Implement this in subclass
}

ERPGDamageZone IRPGDamagableInterface::GetDamageZoneByBoneName_Implementation(const FName& boneName) const
{
	return ERPGDamageZone::ENormal;
}




