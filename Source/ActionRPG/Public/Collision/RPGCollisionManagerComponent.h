// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGCollisionTypes.h"
#include "Components/ActorComponent.h"
#include "RPGCollisionManagerComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(RPGCollisionsManagerComponentLog, Verbose, All);

UCLASS(ClassGroup=(ActionRPG), meta=(BlueprintSpawnableComponent))
class ACTIONRPG_API URPGCollisionManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGCollisionManagerComponent();

protected: // UActorComponent interface
	virtual void BeginPlay() override;
	virtual  void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void SetCollisionsEnabled(bool bStarted);
	void UpdateCollisions();
	
	UFUNCTION()
	void HandleTimedSingleTraceFinished(const FName& traceEnded);

	UFUNCTION()
	void HandleAllTimedTraceFinished();

public:
	UFUNCTION(BlueprintCallable, Category = LightSouls)
	void SetupCollisionManager(class UMeshComponent* inDamageMesh);

	UFUNCTION(BlueprintCallable, Category = LightSouls)
	void AddActorToIgnore(class AActor* ignoredActor);

	UFUNCTION(BlueprintCallable, Category = LightSouls)
	void AddCollisionChannel(const TEnumAsByte<ECollisionChannel>& inTraceChannel);

	UFUNCTION(BlueprintCallable, Category = LightSouls)
	void SetCollisionChannels(const TArray<TEnumAsByte<ECollisionChannel>>& inTraceChannels);

	UFUNCTION(BlueprintCallable, Category = LightSouls)
	void ClearCollisionChannels();
	
	void StartAllTraces();
	void StopAllTraces();
	void StartSingleTrace(const FName& name);
	void StopSingleTrace(const FName& name);
	void StartTimedSingleTrace(const FName& traceName, float duration);
	void StartAllTimedTraces(float duration);

public:

	//~ Client-server communication start
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG|Collision Manager Component")
	void Server_StartAllTraces();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG|Collision Manager Component")
	void Server_StopAllTraces();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG|Collision Manager Component")
	void Server_StartSingleTrace(const FName& name);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG|Collision Manager Component")
	void Server_StopSingleTrace(const FName& name);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG|Collision Manager Component")
	void Server_StartTimedSingleTrace(const FName& traceName, float duration);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "RPG|Collision Manager Component")
	void Server_StartAllTimedTraces(float duration);
	//~ Client-server communication end

public:
	UFUNCTION(BlueprintCallable, Category = "RPG|Collision Manager Component")
	const TMap<FName, FRPGTraceInfo>& GetDamageTraces() const;

public:

	FRPGCollisionDetectedNativeDelegate& GetCollisionDetectedNativeDelegate();

	UFUNCTION(BlueprintCallable, Category = "RPG|Collision Manager Component")
	void SetTraceConfig(const FName& traceName, const FRPGTraceInfo& traceInfo);

	UFUNCTION(BlueprintCallable, Category = "RPG|Collision Manager Component")
	bool TryGetTraceConfig(const FName& traceName, FRPGTraceInfo& outTraceInfo) const;

	void UpdateCollisionSettings(const struct FRPGCollisionInfo& collisionData);

	class UMeshComponent* GetMeshComponent() const;

private:
	FVector ComputeLocationsAtStep(const FVector& previous, const FVector& current, const int32 curStep, const int32 totalSteps);
	void OnTraceCompleted(const FTraceHandle& handle, FTraceDatum& data);
	void AddHitActorToSet(AActor* actor, const FName& traceName, TMap<FName, FRPGHitActorsInfo>& outHitActorInfo);

protected:
	UPROPERTY(BlueprintAssignable, Category = LightSouls)
	FRPGCollisionDetectedDelegate OnCollisionDetected;

	FRPGCollisionDetectedNativeDelegate CollisionDetectedNativeDelegate;

	UPROPERTY(Transient)
	TMap<FName, FRPGTraceInfo> DamageTraces;

	UPROPERTY(Transient)
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels;

	// The actors to be ignore
	UPROPERTY(Transient)
	TArray<class AActor*> IgnoredActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FRPGCollisionInfo CollisionInfo;

	// If we allow hitting the same actor multiple times per swing
	bool bAllowMultipleHitsPerSwing = false;

	bool bDrawDebug = false;

private:
	UPROPERTY(Transient)
	class AActor* ActorOwner;

	UPROPERTY(Transient)
	class UMeshComponent* DamageMesh;

	UPROPERTY(Transient)
	TMap<FName, FRPGTraceInfo> ActivatedTraces;

	TArray<FName> TracesPendingDelete;

	UPROPERTY(Transient)
	TMap<FName, FRPGHitActorsInfo> AlreadyHitActors;

	UPROPERTY(Transient)
	TMap<FName, FTimerHandle> AllTraceTimers;

	TMap<FName, FVector> PreviousStartPositions;
	TMap<FName, FVector> PreviousEndPositions;

	FTimerHandle AllTraceTimer;

	FRPGCollisionManagerTraceDelegate TraceDelegate;

	TMap<FTraceHandle, FName> TraceHandleToCurrentTrace;

	// Each actor should not be hit more than once in each FLSTraceInfo in one frame
	// Regardless of bAllowMultipleHitsPerSwing
	UPROPERTY(Transient)
	TMap<FName, FRPGHitActorsInfo> HitActorsInThisFrame;

	uint8 bSingleTimedTraceStarted : 1;
	uint8 bAllTimedTraceStarted : 1;
	uint8 bHasCollisionsEnabled : 1;
};
