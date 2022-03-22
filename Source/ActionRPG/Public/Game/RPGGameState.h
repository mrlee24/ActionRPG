// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameState.h"
#include "RPGGameState.generated.h"

UCLASS()
class ACTIONRPG_API ARPGGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	// Initialize Default values
	ARPGGameState();
	
protected: // AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:

	//@ return Master Collision component
	class URPGCollisionMasterComponent* GetMasterCollisionComponent() const;
	
protected:
	// Reference to our Collision Master Component that will update collisions for each of our Collision Managers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="RPG|Collision Master Component")
	class URPGCollisionMasterComponent* MasterCollisionComponent = nullptr;
};
