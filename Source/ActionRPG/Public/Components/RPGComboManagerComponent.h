// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGComboManagerComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(RPGComboManagerComponentLog, Log, All);

UCLASS(ClassGroup=(ActionRPG), meta=(BlueprintSpawnableComponent))
class ACTIONRPG_API URPGComboManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGComboManagerComponent();

protected: // UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void OnRegister() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** Setup GetOwner to character and sets references for ability system component and the owner itself. */
	void SetupOwner();

	/** Get the currently active combo ability */
	class UGameplayAbility* GetCurrentActiveComboAbility() const;
	
	void SetComboIndex(const int32 comboIndex);
	
	/** Returns true if this component's actor has authority */
	virtual bool IsOwnerActorAuthoritative() const;
	
	/** Part of the combo system, will increment the ComboIndex counter, only if the combo window has been opened */
	UFUNCTION(BlueprintCallable, Category="GAS Companion|Combat")
	void IncrementCombo();

	/** Part of the combo system, will reset the ComboIndex counter to 0 */
	UFUNCTION(BlueprintCallable, Category="GAS Companion|Combat")
	void ResetCombo();

	/** Part of the combo system, gate combo ability activation based on if character is already using the ability */
	UFUNCTION(BlueprintCallable, Category="GAS Companion|Combat")
	void ActivateComboAbility(TSubclassOf<class URPGGameplayAbility_MeleeBase> abilityClass, bool bAllowRemoteActivation = true);

private:
	/** Caches the flags that indicate whether this component has network authority. */
	void CacheIsNetSimulated();

protected:
	UFUNCTION(Server, Reliable)
	void Server_ActivateComboAbility(TSubclassOf<URPGGameplayAbility_MeleeBase> meleeAbilityClass, bool bAllowRemoteActivation = true);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_ActivateComboAbility(TSubclassOf<URPGGameplayAbility_MeleeBase> meleeAbilityClass, bool bAllowRemoteActivation = true);

	void Internal_ActivateComboAbility(TSubclassOf<URPGGameplayAbility_MeleeBase> meleeAbilityClass, bool bAllowRemoteActivation = true);

	UFUNCTION(Server, Reliable)
	void Server_SetComboIndex(const int32 comboIndex);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_SetComboIndex(const int32 comboIndex);
	
protected:
	UPROPERTY(Transient, DuplicateTransient, BlueprintReadOnly, Category = "RPG|Combo Manager Component")
	class APawn* OwnerPawn = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Combo Manager Component")
	class URPGAbilitySystemComponent* OwnerAbilitySystemComponent = nullptr;

	/** Reference to GA_GSC_Melee_Base */
	TSubclassOf<class URPGGameplayAbility_MeleeBase> MeleeBaseAbility;

public:
	/** The combo index for the currently active combo */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "GAS Companion|Combo")
	int32 ComboIndex = 0;

	/** Whether or not the combo window is opened (eg. player can queue next combo within this window) */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "GAS Companion|Combo")
	uint8 bComboWindowOpened : 1;

	/** Should we queue the next combo montage for the currently active combo */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "GAS Companion|Combo")
	uint8 bShouldTriggerCombo : 1;

	/** Should we trigger the next combo montage */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "GAS Companion|Combo")
	uint8 bRequestTriggerCombo : 1;

	/** Should we trigger the next combo montage */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "GAS Companion|Combo")
	uint8 bNextComboAbilityActivated : 1;

protected:
	UPROPERTY()
	uint8 bCachedIsNetSimulated : 1;
};
