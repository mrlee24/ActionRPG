// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/RPGAbilityBPLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/RPGAbilityDataTypes.h"

const TArray<FGameplayAttribute>& URPGAbilityBPLibrary::GetAllGameplayAttributes()
{
	static TArray<FGameplayAttribute> attributes = FindGameplayAttributes();
	return attributes;
}

float URPGAbilityBPLibrary::GetAttributeValue(const AActor* actor, const FGameplayAttribute& attribute,
	const float defaultIfNotFound)
{
	bool bFound = false;
	const float value = UAbilitySystemBlueprintLibrary::GetFloatAttribute(actor, attribute, bFound);
	if (bFound)
	{
		return value;
	}

	return defaultIfNotFound;
}

void URPGAbilityBPLibrary::GetTags(const AActor* actor, FGameplayTagContainer& outGameplayTags)
{
	outGameplayTags = outGameplayTags.EmptyContainer;
	if (!IsValid(actor))
	{
		return;
	}

	const UAbilitySystemComponent* abilitySystemComponent = actor->FindComponentByClass<UAbilitySystemComponent>();
	if (!abilitySystemComponent)
	{
		return;
	}

	abilitySystemComponent->GetOwnedGameplayTags(outGameplayTags);
}

void URPGAbilityBPLibrary::CreateGameplayEventDataWithEventTag(AActor* source, AActor* target, FGameplayTag eventTag,
	FGameplayEventData& outEventData)
{
	if (!source || !target)
	{
		return;
	}

	FGameplayTagContainer sourceTags;
	GetTags(source, sourceTags);

	FGameplayTagContainer targetTags;
	GetTags(target, targetTags);

	outEventData.EventTag = eventTag;
	outEventData.Instigator = source;
	outEventData.Target = target;
	outEventData.OptionalObject = nullptr;
	outEventData.OptionalObject2 = nullptr;
	outEventData.InstigatorTags = MoveTemp(sourceTags);
	outEventData.TargetTags = MoveTemp(targetTags);
	outEventData.TargetData = nullptr;
}

int32 URPGAbilityBPLibrary::SendGameplayEvent(AActor* actor, FGameplayEventData payload)
{
	if (IsValid(actor))
	{
		IAbilitySystemInterface* abilitySystemInterfac = Cast<IAbilitySystemInterface>(actor);
		if (abilitySystemInterfac)
		{
			UAbilitySystemComponent* abilitySystemComponent = abilitySystemInterfac->GetAbilitySystemComponent();
			if (abilitySystemComponent)
			{
				FScopedPredictionWindow newScopedWindow(abilitySystemComponent, true);
				return abilitySystemComponent->HandleGameplayEvent(payload.EventTag, &payload);
			}
		}
	}

	return 0;
}

FGameplayAbilityTargetDataHandle URPGAbilityBPLibrary::EffectContextGetTargetData(
	FGameplayEffectContextHandle effectContextHandle)
{
	const FRPGGameplayEffectContext* gameplayEffectContext = static_cast<FRPGGameplayEffectContext*>(effectContextHandle.Get());
	if (gameplayEffectContext)
	{
		return gameplayEffectContext->GetTargetData();
	}

	return FGameplayAbilityTargetDataHandle();
}

void URPGAbilityBPLibrary::EffectContextAddTargetData(FGameplayEffectContextHandle effectContextHandle,
	const FGameplayAbilityTargetDataHandle& targetData)
{
	FRPGGameplayEffectContext* gameplayEffectContext = static_cast<FRPGGameplayEffectContext*>(effectContextHandle.Get());
	if (gameplayEffectContext)
	{
		gameplayEffectContext->AddTargetData(targetData);
	}
}

TArray<FGameplayAttribute> URPGAbilityBPLibrary::FindGameplayAttributes()
{
	TArray<FGameplayAttribute> attributes;
	for (TObjectIterator<UClass> classIt; classIt; ++classIt)
	{
		UClass* Class = *classIt;
		if (Class->IsChildOf(UAttributeSet::StaticClass()) && !Class->ClassGeneratedBy)
		{
			for (TFieldIterator<FProperty> propertyIt(Class, EFieldIteratorFlags::ExcludeSuper); propertyIt;
				 ++propertyIt)
			{
				FProperty* property = *propertyIt;
				attributes.Add(FGameplayAttribute(property));
			}
		}
	}

	return attributes;
}
