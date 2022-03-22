// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/RPGAsyncTaskAttributeChanged.h"

#include "AbilitySystemComponent.h"

URPGAsyncTaskAttributeChanged* URPGAsyncTaskAttributeChanged::ListenForAttributeChange(
	UAbilitySystemComponent* abilitySystemComponent, const FGameplayAttribute& attribute)
{
	URPGAsyncTaskAttributeChanged* waitForAttributeChangedTask = NewObject<URPGAsyncTaskAttributeChanged>();
	waitForAttributeChangedTask->AbilitySystemComponent = abilitySystemComponent;
	waitForAttributeChangedTask->AttributeToListenFor = attribute;

	if (!IsValid(abilitySystemComponent) || !attribute.IsValid())
	{
		waitForAttributeChangedTask->RemoveFromRoot();
		return nullptr;
	}

	abilitySystemComponent->GetGameplayAttributeValueChangeDelegate(attribute)
		.AddUObject(waitForAttributeChangedTask, &ThisClass::OnAttributeChanged);
	return waitForAttributeChangedTask;
}

URPGAsyncTaskAttributeChanged* URPGAsyncTaskAttributeChanged::ListenForAttributesChange(
	UAbilitySystemComponent* abilitySystemComponent, const TArray<FGameplayAttribute>& attributes)
{
	URPGAsyncTaskAttributeChanged* waitForAttributeChangedTask = NewObject<URPGAsyncTaskAttributeChanged>();
	waitForAttributeChangedTask->AbilitySystemComponent = abilitySystemComponent;
	waitForAttributeChangedTask->AttributesToListenFor = attributes;

	if (!IsValid(abilitySystemComponent) || attributes.Num() < 1)
	{
		waitForAttributeChangedTask->RemoveFromRoot();
		return nullptr;
	}

	for (const FGameplayAttribute& gameplayAttribute : attributes)
	{
		abilitySystemComponent->GetGameplayAttributeValueChangeDelegate(gameplayAttribute).
			AddUObject(waitForAttributeChangedTask, &ThisClass::OnAttributeChanged);
	}

	return waitForAttributeChangedTask;
}

void URPGAsyncTaskAttributeChanged::EndTask()
{
	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);

		for (const FGameplayAttribute& gameplayAttribute : AttributesToListenFor)
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(gameplayAttribute).RemoveAll(this);
		}
	}

	SetReadyToDestroy();
	MarkPendingKill();
}

void URPGAsyncTaskAttributeChanged::OnAttributeChanged(const FOnAttributeChangeData& data) const
{
	AttributeChanged.Broadcast(data.Attribute, data.NewValue, data.OldValue);
}
