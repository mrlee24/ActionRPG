// Fill out your copyright notice in the Description page of Project Settings.


#include "Attributes/RPGAttributeSet_Health.h"

#include "Interfaces/RPGDamagableInterface.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"

URPGAttributeSet_Health::URPGAttributeSet_Health()
{
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitDamageApplied(0.f);
}

void URPGAttributeSet_Health::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
}

void URPGAttributeSet_Health::PreAttributeChange(const FGameplayAttribute& attribute, float& newValue)
{
	Super::PreAttributeChange(attribute, newValue);
	
	if (attribute == GetHealthAttribute())
	{
		newValue = FMath::Min(newValue, MaxHealth.GetCurrentValue());
	}
}

void URPGAttributeSet_Health::PreAttributeBaseChange(const FGameplayAttribute& attribute, float& newValue) const
{
	Super::PreAttributeBaseChange(attribute, newValue);

	if (attribute == GetHealthAttribute())
	{
		newValue = FMath::Clamp(newValue, 0.f, MaxHealth.GetCurrentValue());
	}
}

void URPGAttributeSet_Health::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& data)
{
	Super::PostGameplayEffectExecute(data);
	const FGameplayEffectContextHandle effectContextHandle = data.EffectSpec.GetContext();
	const FGameplayTagContainer& sourceTags = *data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	UAbilitySystemComponent* source = effectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	FGameplayTagContainer specAssetTags;
	data.EffectSpec.GetAllAssetTags(specAssetTags);

	// Get the Target actor, which should be our owner
	AActor* targetActor = nullptr;
	if (data.Target.AbilityActorInfo.IsValid() && data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		targetActor = data.Target.AbilityActorInfo->AvatarActor.Get();
	}

	// Get the Source actor
	AActor* sourceActor = nullptr;
	AController* sourceController = nullptr;
	APawn* sourcePawn = nullptr;
	if (source && source->AbilityActorInfo.IsValid() && source->AbilityActorInfo->AvatarActor.IsValid())
	{
		sourceActor = source->AbilityActorInfo->AvatarActor.Get();
		sourceController = source->AbilityActorInfo->PlayerController.Get();
		if (sourceController == nullptr && sourceActor != nullptr)
		{
			if (APawn* pawn = Cast<APawn>(sourceActor))
			{
				sourceController = pawn->GetController();
			}
		}

		// Use the controller to find the source pawn
		if (sourceController)
		{
			sourcePawn = sourceController->GetPawn();
		}
		else
		{
			sourcePawn = Cast<APawn>(sourceActor);
		}

		// Set the causer actor based on context if it's set
		if (effectContextHandle.GetEffectCauser())
		{
			sourceActor = effectContextHandle.GetEffectCauser();
		}
	}

	if (data.EvaluatedData.Attribute == GetDamageAppliedAttribute()
		/*&& !data.Target.HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(GTag_Cheat_GodMode))*/)
	{
		// Store a local copy of the amount of damage done and clear the damage attribute
		const float localDamageDone = GetDamageApplied();

		SetDamageApplied(0.f); // Reset Damage Applied!!!

		// If character was alive before damage is added, handle damage
		// This prevents damage being added to dead things and replaying death animations
		bool bWasAlive = true;

		IRPGDamagableInterface* damagableInterface = Cast<IRPGDamagableInterface>(targetActor);
		const float newHealth = GetHealth() - localDamageDone;
		SetHealth(FMath::Clamp(newHealth, 0.0f, GetMaxHealth()));

		if (damagableInterface)
		{
			bWasAlive = damagableInterface->Execute_IsAlive(targetActor);
			FRPGDamageEvent damageEvent;
			damageEvent.Damage = localDamageDone;
			damageEvent.InstigatorController = sourceController;
			damageEvent.InstigatorActor = sourceActor;
			damageEvent.TargetActor = targetActor;
			damageEvent.bIsFinalBlow = bWasAlive && GetHealth() <= 0;
			damageEvent.HitResult = { MoveTemp(const_cast<FHitResult&>(*effectContextHandle.GetHitResult())) };
			damagableInterface->OnTakeDamage(MoveTemp(damageEvent));
		}

		//if (implementsKillableInterface && bWasAlive)
		//{
		//	// TODO: Implement support for giving XP to the player here
		//}
	} // Damage
	else if (data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Handle other health changes.
		// Health loss should go through Damage.
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	} // Health
}

void URPGAttributeSet_Health::OnRep_Health(const FGameplayAttributeData& oldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, oldHealth);
}

void URPGAttributeSet_Health::OnRep_MaxHealth(const FGameplayAttributeData& oldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, oldMaxHealth);
}
