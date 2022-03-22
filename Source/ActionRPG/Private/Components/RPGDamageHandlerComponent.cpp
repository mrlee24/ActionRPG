// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGDamageHandlerComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interfaces/RPGDamagableInterface.h"
#include "Interfaces/RPGSpawnableInterface.h"
#include "Net/UnrealNetwork.h"


class IRPGSpawnableInterface;
// Sets default values for this component's properties
URPGDamageHandlerComponent::URPGDamageHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void URPGDamageHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (GetOwner())
	{
		AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
		if (AbilitySystemComponent.IsValid())
		{
			// AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			// 	URPGAttributeSetHealth::GetHealthAttribute()).AddUObject(this, &URPGDamageHandlerComponent::OnHealthAttributeChanged);
		}
	}
}

void URPGDamageHandlerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsAlive);
}

void URPGDamageHandlerComponent::OnHealthAttributeChanged(const FOnAttributeChangeData& attributeData)
{
	bIsAlive = attributeData.NewValue > 0.f;

	OnHealthChangedDelegate.Broadcast(attributeData.NewValue, attributeData.OldValue);
	OnHealthChangedNativeDelegate.Broadcast(attributeData.NewValue, attributeData.OldValue);
}

void URPGDamageHandlerComponent::OnRep_IsAlive()
{
}

void URPGDamageHandlerComponent::OnTakeDamage(const FRPGDamageEvent& damageEvent)
{
	// Begin Debug: Server report damage received
#if WITH_EDITOR
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, FString::Printf(TEXT("%s took damage %f!"), *(GetOwner()->GetActorLabel()), damageEvent.Damage));
		if (damageEvent.bIsFinalBlow)
		{
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, FString::Printf(TEXT("And It was the final blow!")));
		}
	}
#endif
	// End Debug

	// Broadcast to everyone an RPC that damage has been dealt, this is for all the players that are within the NetRelevancyRange of the owner of this component
	NetMulticast_ReceivedDamage(damageEvent);
}

void URPGDamageHandlerComponent::StartDelayedDestruction()
{
	if (GetWorld() && GetWorld()->IsServer())
	{
		if (bAutoDestroyOnDeath && ActorLifeSpanOnDeath > 0.f)
		{
			GetWorld()->GetTimerManager().SetTimer
			(
				TimerHandle_DelayedDestructionCallback,
				FTimerDelegate::CreateWeakLambda
				(
					this, [this]()
					{
						DestroyEntity();
					}
				), ActorLifeSpanOnDeath, false
			);
		}
	}
}

void URPGDamageHandlerComponent::RequestDestructionNow()
{
	if (GetWorld() && GetWorld()->IsServer())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_DelayedDestructionCallback);
		DestroyEntity();
	}
}

bool URPGDamageHandlerComponent::IsAlive() const
{
	return bIsAlive;
}

FRPGEntityDeathNativeDelegate& URPGDamageHandlerComponent::GetDeathNativeDelegate()
{
	return OnEntityDeathNativeDelegate;
}

FRPGHealthChangedNativeDelegate& URPGDamageHandlerComponent::GetHealthChangedNativeDelegate()
{
	return OnHealthChangedNativeDelegate;
}

void URPGDamageHandlerComponent::NetMulticast_ReceivedDamage_Implementation(const FRPGDamageEvent& damageEvent)
{
	IRPGDamagableInterface* damagableInterface = Cast<IRPGDamagableInterface>(damageEvent.TargetActor);
	if (damagableInterface)
	{
		damagableInterface->Execute_PerformHitReaction(damageEvent.TargetActor, damageEvent);
	}

	if (damageEvent.bIsFinalBlow)
	{
		// If is final blow, call kill on each client and server
		// Some logic should only run on the server, this need to be handled inside the implementation of Kill function
		if (damagableInterface)
		{
			damagableInterface->Execute_Kill(damageEvent.TargetActor, damageEvent.InstigatorActor);
		}

		FRPGEntityDeathDelegateData payload;
		payload.DamageEvent = damageEvent;

		OnEntityDeathDelegate.Broadcast(payload);
		OnEntityDeathNativeDelegate.Broadcast(payload);
	}
}

void URPGDamageHandlerComponent::Client_DealDamage_Implementation(const FRPGDamageEvent& damageEvent)
{
	if (damageEvent.TargetActor != damageEvent.InstigatorActor)
	{
		// Notify on UI here
	}
}

void URPGDamageHandlerComponent::DestroyEntity()
{
	if (!GetWorld() || !GetWorld()->IsServer())
	{
		// Destroy should never be called on the client's
		ensure(false);
		return;
	}

	// If the owner does implement our SpawnableInterface then it means that it might have been spawned by a spawner
	// hence instead of calling Destroy directly here we route that through the interface
	IRPGSpawnableInterface* spawnable = Cast<IRPGSpawnableInterface>(GetOwner());
	if(spawnable /*&& spawnable->GetSpawner()*/)
	{
		IRPGSpawnableInterface::Execute_DespawnSpawnable(GetOwner(), this, ERPGEndPlayReason::Destroyed);
	}
	else
	{
		GetOwner()->Destroy();
	}
}

