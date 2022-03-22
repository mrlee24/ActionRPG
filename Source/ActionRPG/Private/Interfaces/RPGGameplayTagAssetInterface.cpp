// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/RPGGameplayTagAssetInterface.h"

#include "GameplayTagContainer.h"


// Add default functionality here for any IRPGGameplayTagAssetInterface functions that are not pure virtual.
bool IRPGGameplayTagAssetInterface::HasAllMatchingGameplayTagsExact(const FGameplayTagContainer& tagContainer) const
{
	FGameplayTagContainer ownedTags;
	GetOwnedGameplayTags(ownedTags);

	return ownedTags.HasAllExact(tagContainer);
}

bool IRPGGameplayTagAssetInterface::HasAnyMatchingGameplayTagsExact(const FGameplayTagContainer& tagContainer) const
{
	FGameplayTagContainer ownedTags;
	GetOwnedGameplayTags(ownedTags);

	return ownedTags.HasAnyExact(tagContainer);
}
