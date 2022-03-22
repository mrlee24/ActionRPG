// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGCollisionMasterComponent.generated.h"


UCLASS(ClassGroup=(ActionRPG), meta=(BlueprintSpawnableComponent))
class ACTIONRPG_API URPGCollisionMasterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGCollisionMasterComponent();

public:
	
	// Called every frame
	virtual void TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;

	void AddCollisionManagerComponent(class URPGCollisionManagerComponent* componentToAdd);
	void RemoveCollisionManagerComponent(class URPGCollisionManagerComponent* componentToRemove);
	
private:
	
	UPROPERTY(Transient)
	TSet<class URPGCollisionManagerComponent*> ActiveComponents;

	UPROPERTY(Transient)
	TSet<class URPGCollisionManagerComponent*> PendingDeleteComponents;
};
