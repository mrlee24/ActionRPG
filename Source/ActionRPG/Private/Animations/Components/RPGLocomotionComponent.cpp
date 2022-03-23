// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Components/RPGLocomotionComponent.h"

#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
URPGLocomotionComponent::URPGLocomotionComponent(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void URPGLocomotionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CurrentLocomotionState = *LocomotionStates.Find(DefaultLocomotionStateName);
}

void URPGLocomotionComponent::PostInitProperties()
{
	Super::PostInitProperties();
	
	bUseCharacterInstance = false;
	DefaultLocomotionStateName = GTag_Movement_CustomState_Jog;
	
	LocomotionStates.Add(GTag_Movement_CustomState_Crouch,
		FRPGLocomotionState(FGameplayTag::RequestGameplayTag(GTag_Movement_CustomState_Crouch), 150.f));
	LocomotionStates.Add(GTag_Movement_CustomState_Walk,
	FRPGLocomotionState(FGameplayTag::RequestGameplayTag(GTag_Movement_CustomState_Walk), 180.f));
	LocomotionStates.Add(GTag_Movement_CustomState_Jog,
		FRPGLocomotionState(FGameplayTag::RequestGameplayTag(GTag_Movement_CustomState_Jog), 350.f));
}

void URPGLocomotionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(URPGLocomotionComponent, CurrentLocomotionState);
}

void URPGLocomotionComponent::SetUseCharacterInstance(const bool useCharacterInstance)
{
	bUseCharacterInstance = useCharacterInstance;
}

FRPGLocomotionStateNativeChanged& URPGLocomotionComponent::GetLocomotionStateNativeChanged()
{
	return LocomotionStateNativeChangedDelegate;
}

const FGameplayTag& URPGLocomotionComponent::GetCurrentLocomotionStateTag() const
{
	return CurrentLocomotionState.State;
}

void URPGLocomotionComponent::SetupCurrentLocomotionState(const FName& locomotionStateName)
{
	CurrentLocomotionState = *LocomotionStates.Find(locomotionStateName);
}

float URPGLocomotionComponent::GetMaxSpeedByStateName(const FName& locomotionStateName) const
{
	const FRPGLocomotionState* locomotionState = LocomotionStates.Find(locomotionStateName);
	if (locomotionState)
	{
		return locomotionState->MaxSpeed;
	}

	return 0.f;
}

void URPGLocomotionComponent::Server_SetLocomotionState_Implementation(const FName& locomotionStateName)
{
	const FRPGLocomotionState* newLocomotionState = LocomotionStates.Find(locomotionStateName);
	if (newLocomotionState)
	{
		CurrentLocomotionState = *newLocomotionState;
	}
}

bool URPGLocomotionComponent::Server_SetLocomotionState_Validate(const FName& locomotionStateName)
{
	return true;
}

void URPGLocomotionComponent::NotifyLocomotionState(const FRPGLocomotionState& newLocomotionState)
{
	LocomotionStateChangedDelegate.Broadcast(newLocomotionState);
	LocomotionStateNativeChangedDelegate.Broadcast(newLocomotionState);
}

void URPGLocomotionComponent::OnRep_LocomotionState(const FRPGLocomotionState& newLocomotionState)
{
	NotifyLocomotionState(newLocomotionState);
}
