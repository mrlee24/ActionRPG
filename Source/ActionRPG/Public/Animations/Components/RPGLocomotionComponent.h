// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGAnimMasterComponent.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RPGLocomotionComponent.generated.h"

USTRUCT(BlueprintType)
struct FRPGLocomotionState
{
	GENERATED_BODY()

	FRPGLocomotionState() {};

	FRPGLocomotionState(FGameplayTag state, float maxSpeed)
	{
		State = state;
		MaxSpeed = maxSpeed;
	};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF")
	FGameplayTag State;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF")
	float MaxSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF")
	FName CameraMovement;

	FORCEINLINE	bool operator < (const FRPGLocomotionState& other) const
	{
		return MaxSpeed < other.MaxSpeed;
	}

	FORCEINLINE	bool operator == (const FRPGLocomotionState& other) const
	{
		return State == other.State;
	}

	FORCEINLINE	bool operator != (const FGameplayTag& other) const
	{
		return State != other;
	}

	FORCEINLINE	bool operator == (const FGameplayTag& other) const
	{
		return State == other;
	}

	FORCEINLINE	bool operator != (const FRPGLocomotionState& other) const
	{
		return State != other.State;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGLocomotionStateChanged, const FRPGLocomotionState&, locomotionState);
DECLARE_MULTICAST_DELEGATE_OneParam(FRPGLocomotionStateNativeChanged, const FRPGLocomotionState&);

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
	
public:
	void SetUseCharacterInstance(const bool useCharacterInstance = false);
	FRPGLocomotionStateNativeChanged& GetLocomotionStateNativeChanged();

public:
	UFUNCTION(BlueprintPure, Category = "RPGLocomotionComponent|Locomotion")
	const FGameplayTag& GetCurrentLocomotionStateTag() const;

	UFUNCTION(BlueprintCallable, Category = "RPGLocomotionComponent|Locomotion")
	void SetupCurrentLocomotionState(const FName& locomotionStateName);

	UFUNCTION(BlueprintPure, Category = "RPGLocomotionComponent|Locomotion")
	float GetMaxSpeedByStateName(const FName& locomotionStateName) const;

private:
	UFUNCTION(Server, WithValidation, Reliable)
	void Server_SetLocomotionState(const FName& locomotionStateName);
	
private:
	void NotifyLocomotionState(const FRPGLocomotionState& newLocomotionState);
		
	UFUNCTION()
	void OnRep_LocomotionState(const FRPGLocomotionState& newLocomotionState);
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_LocomotionState)
	FRPGLocomotionState CurrentLocomotionState;
	
protected:
	// This is to indicate which Movement Component will be used
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPGLocomotionComponent|Setup")
	uint8 bUseCharacterInstance : 1;

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
