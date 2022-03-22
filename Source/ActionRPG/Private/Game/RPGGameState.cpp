// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RPGGameState.h"

#include "Collision/RPGCollisionMasterComponent.h"

ARPGGameState::ARPGGameState() : Super()
{
	MasterCollisionComponent = CreateDefaultSubobject<URPGCollisionMasterComponent>("CollisionMasterComponent");
}

// Called when the game starts or when spawned
void ARPGGameState::BeginPlay()
{
	Super::BeginPlay();

}

void ARPGGameState::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);
}

void ARPGGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

URPGCollisionMasterComponent* ARPGGameState::GetMasterCollisionComponent() const
{
	return MasterCollisionComponent;
}


