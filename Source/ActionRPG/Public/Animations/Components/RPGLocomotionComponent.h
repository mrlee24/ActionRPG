// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGAnimMasterComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RPGLocomotionComponent.generated.h"

UCLASS(ClassGroup=(ActionRPG), meta=(BlueprintSpawnableComponent))
class ACTIONRPG_API URPGLocomotionComponent : public URPGAnimMasterComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGLocomotionComponent(const FObjectInitializer& objectInitializer);

protected: // UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected: // URPGAnimMasterComponent
	virtual void InitComponent() override;
	
public:
	FRPGLocomotionStateNativeChanged& GetLocomotionStateNativeChanged();

public:
	UFUNCTION(BlueprintCallable, Category = "RPGLocomotionComponent|Setup")
	void SetupLocomotionState(const FName& locomotionStateName);
	
	UFUNCTION(BlueprintPure, Category = "RPGLocomotionComponent|Locomotion")
	const FGameplayTag& GetCurrentLocomotionStateTag() const;
	
	UFUNCTION(BlueprintPure, Category = "RPGLocomotionComponent|Locomotion")
	float GetMaxSpeedByStateName(const FName& locomotionStateName) const;

	UFUNCTION(BlueprintPure, Category = "RPGLocomotionComponent|Locomotion")
	bool IsLocomotionStateSet(const FName& locomotionStateName) const;

private:
	UFUNCTION(Server, WithValidation, Reliable)
	void Server_SetLocomotionState(const FRPGLocomotionState& newLocomotionState);
	
private:
	void NotifyLocomotionState(const FRPGLocomotionState& newLocomotionState);
		
	UFUNCTION()
	void OnRep_LocomotionState(const FRPGLocomotionState& newLocomotionState);
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_LocomotionState)
	FRPGLocomotionState CurrentLocomotionState;
	
protected:
	// Default Locomotion State
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPGLocomotionComponent|Setup")
	FName DefaultLocomotionStateName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPGLocomotionComponent|Locomotion")
	TMap<FName, FRPGLocomotionState> LocomotionStates;

public:
	UPROPERTY(BlueprintAssignable, Category = "RPGLocomotionComponent")
	FRPGLocomotionStateChanged LocomotionStateChangedDelegate;	

protected:
	FRPGLocomotionStateNativeChanged LocomotionStateNativeChangedDelegate;
};
