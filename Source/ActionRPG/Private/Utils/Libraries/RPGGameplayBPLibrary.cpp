// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/Libraries/RPGGameplayBPLibrary.h"

#include "GameplayTagsManager.h"

FGameplayTagContainer URPGGameplayBPLibrary::RequestAllGameplayTags(bool bOnlyIncludeDictionaryTags)
{
	FGameplayTagContainer container;
	UGameplayTagsManager::Get().RequestAllGameplayTags(container, bOnlyIncludeDictionaryTags);
	return container;
}
