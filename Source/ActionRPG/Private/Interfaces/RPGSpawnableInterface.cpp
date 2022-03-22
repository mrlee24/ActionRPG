// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/RPGSpawnableInterface.h"


// Add default functionality here for any IRPGSpawnableInterface functions that are not pure virtual.
void IRPGSpawnableInterface::DespawnSpawnable_Implementation(const UObject* instigator,
	const ERPGEndPlayReason::Type endReason)
{
	// @techdebt: Implement this when we have SpawnerEntity
}
