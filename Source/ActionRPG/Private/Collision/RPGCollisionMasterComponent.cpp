// Fill out your copyright notice in the Description page of Project Settings.


#include "Collision/RPGCollisionMasterComponent.h"

#include "Collision/RPGCollisionManagerComponent.h"

DECLARE_STATS_GROUP(TEXT("RPGCollisionMasterComponent"), STATGROUP_RPGCollisionMaster, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("RPGCollisionMasterComponent - TickComponent"), STAT_RPGCollisionMaster_TickComponent,
STATGROUP_RPGCollisionMaster);

// Sets default values for this component's properties
URPGCollisionMasterComponent::URPGCollisionMasterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called every frame
void URPGCollisionMasterComponent::TickComponent(float deltaTime, ELevelTick tickType,
                                                 FActorComponentTickFunction* thisTickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_RPGCollisionMaster_TickComponent);

	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	for (URPGCollisionManagerComponent* collisionManagerComponent : PendingDeleteComponents)
	{
		ActiveComponents.Remove(collisionManagerComponent);
	}

	PendingDeleteComponents.Reset();

	for (URPGCollisionManagerComponent* collisionManagerComponent : ActiveComponents)
	{
		if (collisionManagerComponent)
		{
			collisionManagerComponent->UpdateCollisions();
		}
		else
		{
			PendingDeleteComponents.Add(collisionManagerComponent);
		}
	}
}

void URPGCollisionMasterComponent::AddCollisionManagerComponent(URPGCollisionManagerComponent* componentToAdd)
{
	ActiveComponents.Add(componentToAdd);

	if (PendingDeleteComponents.Contains(componentToAdd))
	{
		PendingDeleteComponents.Remove(componentToAdd);
	}
}

void URPGCollisionMasterComponent::RemoveCollisionManagerComponent(URPGCollisionManagerComponent* componentToRemove)
{
	if (ActiveComponents.Contains(componentToRemove))
	{
		PendingDeleteComponents.Add(componentToRemove);
	}
}

