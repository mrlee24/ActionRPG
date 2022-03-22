// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "RPGAbilityDataTypes.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "UObject/Object.h"
#include "RPGAsyncTaskAttributeChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRPGAttributeChangedDelegate,const FGameplayAttribute&, attribute, const float, newValue, const float, oldValue);

/**
* Blueprint node to automatically register a listener for all attribute changes in an AbilitySystemComponent.
* Useful to use in UI.
 */
UCLASS()
class ACTIONRPG_API URPGAsyncTaskAttributeChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	// Listens for an attribute changing.
	static URPGAsyncTaskAttributeChanged* ListenForAttributeChange(
		class UAbilitySystemComponent * abilitySystemComponent, 
		const FGameplayAttribute& attribute);

	// Version that takes in an array of Attributes. Check the Attribute output for which Attribute changed.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static URPGAsyncTaskAttributeChanged* ListenForAttributesChange(
		UAbilitySystemComponent* abilitySystemComponent, 
		const TArray<FGameplayAttribute>& attributes);

	// You must call this function manually when you want the AsyncTask to end.
	// For UMG Widgets, you would call it in the Widget's Destruct event.	
	UFUNCTION(BlueprintCallable)
	void EndTask();

private:
	// Callback called whenever an attribute changes its value on the AbilitySystemComponent
	void OnAttributeChanged(const FOnAttributeChangeData& data) const;

protected:

	// Cached pointer to our ability system component
	UPROPERTY(Transient)
	class UAbilitySystemComponent* AbilitySystemComponent = nullptr;

	UPROPERTY(BlueprintAssignable)
	FRPGAttributeChangedDelegate AttributeChanged;

	FGameplayAttribute AttributeToListenFor;
	TArray<FGameplayAttribute> AttributesToListenFor;
};
