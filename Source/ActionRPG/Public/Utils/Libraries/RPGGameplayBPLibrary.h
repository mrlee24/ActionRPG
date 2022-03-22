// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "RPGGameplayBPLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(RPGGameplayBlueprintLog, Verbose, All);

#define UENUM_TO_STRING(enumClass, enumValue, outStringValue) \
{ \
	const UEnum* const currentEnum = StaticEnum<enumClass>();\
	outStringValue = UKismetNodeHelperLibrary::GetEnumeratorUserFriendlyName(currentEnum, (uint8)enumValue); \
}

/**
 * 
 */
UCLASS()
class ACTIONRPG_API URPGGameplayBPLibrary : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Settings")
	static FGameplayTagContainer RequestAllGameplayTags(bool bOnlyIncludeDictionaryTags);
};
