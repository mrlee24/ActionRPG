// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "UObject/Object.h"
#include "RPGAbilityBPLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API URPGAbilityBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ---------------------------------------------------------------------------------------------------
	// Attributes
	// ---------------------------------------------------------------------------------------------------
	
	/**
	* Gets all game play attributes of all attribute sets (attributes declared in'UAbilitySystemComponent' not
	* included).
	*/
	UFUNCTION(BlueprintPure, Category = "RPG|Gameplay Ability")
	static const TArray<FGameplayAttribute>& GetAllGameplayAttributes();

	/**
	* Gets the value of the specified gameplay attribute or the default value if the attribute could not be found.
	* Note that this function does not need to be exposed to blueprints because the function
	* 'UAbilitySystemBlueprintLibrary::GetFloatAttribute' resolves in a more appropriate blueprint node.
	*/
	UFUNCTION(BlueprintPure, Category = "RPG|Gameplay Ability")
	static float GetAttributeValue(const AActor* actor, const FGameplayAttribute& attribute,
								   const float defaultIfNotFound = 0.f);

	// ---------------------------------------------------------------------------------------------------
	// Tags
	// ---------------------------------------------------------------------------------------------------
	
	/** Gets the gameplay tags of the specified actor. */
	UFUNCTION(BlueprintPure, Category = "RPG|Gameplay Ability")
	static void GetTags(const AActor* actor, FGameplayTagContainer& outGameplayTags);

	// ---------------------------------------------------------------------------------------------------
	// Gameplay Events
	// ---------------------------------------------------------------------------------------------------

	/** Fills a 'FGameplayEventData' with values from the specified Event-Tag without EventData. */
	UFUNCTION(BlueprintPure, Category = "RPG|Gameplay Ability")
	static void CreateGameplayEventDataWithEventTag(AActor* source, AActor* target, FGameplayTag eventTag,
													FGameplayEventData& outEventData);

	/**
	* Sends a gameplay event to the specified actor. Returns the number of successful ability activations triggered by
	* the event.
	*/
	UFUNCTION(BlueprintPure, Category = "RPG|Gameplay Ability")
	static int32 SendGameplayEvent(AActor* actor, FGameplayEventData payload);

	// ---------------------------------------------------------------------------------------------------
	//FTowersGameplayEffectContext
	// ---------------------------------------------------------------------------------------------------

	// Returns TargetData
	UFUNCTION(BlueprintCallable, Category = "Ability|effectContext", Meta = (DisplayName = "GetTargetData"))
	static FGameplayAbilityTargetDataHandle EffectContextGetTargetData(FGameplayEffectContextHandle effectContext);

	// Adds targetData
	UFUNCTION(BlueprintCallable, Category = "Ability|EffectContext", Meta = (DisplayName = "AddTargetData"))
	static void EffectContextAddTargetData(FGameplayEffectContextHandle effectContextHandle, const FGameplayAbilityTargetDataHandle& targetData);
	
private:
	static TArray<FGameplayAttribute> FindGameplayAttributes();
};
