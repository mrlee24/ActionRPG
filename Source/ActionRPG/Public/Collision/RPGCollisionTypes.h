// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RPGCollisionTypes.generated.h"

USTRUCT(BlueprintType)
struct ACTIONRPG_API FRPGHitActorsInfo
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> AlreadyHitActors;
};

UENUM(BlueprintType)
enum class ETowersDamageType : uint8
{
	EPoint UMETA(DisplayName = "Point Damage"),
	EArea UMETA(DisplayName = "Area Damage"),
};

USTRUCT(BlueprintType)
struct ACTIONRPG_API FRPGTraceInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RPG|Trace Info")
	FName StartSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RPG|Trace Info")
	FName EndSocket;

	// The radius of the sphere we use to sweep from base of the weapon to the tip of the weapon
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RPG|Trace Info")
	float Radius = 8.f;

	// The max amount of sub steps allowed for tracing per frame (excluding the final step). 0 means no sub stepping
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RPG|Trace Info")
	int MaxSubSteps = 0;

	// The max distance between the location of EndSocket in two contiguous frames that we use to generate sub steps for tracing
	// If the distance is bigger than SubStepMaxDistance * A, we will generate Min(MaxSubSteps, A) substeps evenly using interpolation.
	// e.g. If SubStepMaxDistance is 10.f and the actual distance is 11.f, we will generate a sub step at location 5.5f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RPG|Trace Info")
	float SubStepMaxDistance = 10.f;
};

USTRUCT(BlueprintType)
struct ACTIONRPG_API FRPGCollisionInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "RPG|Collision Info")
	TMap<FName, FRPGTraceInfo> DamageTraces;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "RPG|Collision Info")
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels;

	// The actors to be ignored
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "RPG|Collision Info")
	TArray<class AActor*> IgnoredActors;

	// If we allow hitting the same actor multiple times per swing
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "RPG|Collision Info")
	bool bAllowMultipleHitsPerSwing = false;

	// If draw debug for collision trace
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "RPG|Collision Info")
	bool bCollisionDrawDebug = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGCollisionDetectedDelegate, const FName&, traceName, const FHitResult&, hitResult);

DECLARE_MULTICAST_DELEGATE_TwoParams(FRPGCollisionDetectedNativeDelegate, const FName&, const FHitResult&/*, hitResult*/);

DECLARE_DELEGATE_TwoParams(FRPGCollisionManagerTraceDelegate, const FTraceHandle&, FTraceDatum&);