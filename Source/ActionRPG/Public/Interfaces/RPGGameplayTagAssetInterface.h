// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "UObject/Interface.h"
#include "RPGGameplayTagAssetInterface.generated.h"

/** Interface for assets which contain game play tags */
UINTERFACE(Blueprintable, MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class URPGGameplayTagAssetInterface : public UGameplayTagAssetInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONRPG_API IRPGGameplayTagAssetInterface : public IGameplayTagAssetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
	virtual void AddGameplayTag(const FGameplayTag& tagToAdd) = 0;

	UFUNCTION(BlueprintCallable, Category = GameplayTags)
	virtual void RemoveGameplayTag(const FGameplayTag& tagToRemove) = 0;

	UFUNCTION(BlueprintCallable, Category = GameplayTags)
	virtual void AddGameplayTags(const FGameplayTagContainer& tagsToAdd) = 0;

	UFUNCTION(BlueprintCallable, Category = GameplayTags)
	virtual void RemoveGameplayTags(const FGameplayTagContainer& tagsToRemove) = 0;

	UFUNCTION(BlueprintCallable, Category = GameplayTags)
	virtual bool HasAllMatchingGameplayTagsExact(const FGameplayTagContainer& tagContainer) const;

	UFUNCTION(BlueprintCallable, Category = GameplayTags)
	virtual bool HasAnyMatchingGameplayTagsExact(const FGameplayTagContainer& tagContainer) const;
};
