// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/Libraries/RPGGameplayBPLibrary.h"

#include "GameplayTagsManager.h"

FGameplayTagContainer URPGGameplayBPLibrary::RequestAllGameplayTags(bool bOnlyIncludeDictionaryTags)
{
	FGameplayTagContainer container;
	UGameplayTagsManager::Get().RequestAllGameplayTags(container, bOnlyIncludeDictionaryTags);
	return container;
}

bool URPGGameplayBPLibrary::IsRunningGameplayWorld(const UWorld* world)
{
	if(ensureAlwaysMsgf(world, TEXT("%s passed world is null"), ANSI_TO_TCHAR(__func__)))
	{
		const EWorldType::Type worldType = world->WorldType;

		// we are running in a gameplay world if the type is Game or PIE.
		// all other scenarios are not considered "gameplay" world
		if(worldType == EWorldType::Game || worldType == EWorldType::PIE)
		{
			return true;
		}
	}
	return false;
}
