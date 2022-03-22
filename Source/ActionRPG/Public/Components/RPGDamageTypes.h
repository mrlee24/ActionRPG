// (c) 2022 Long Ly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "RPGDamageTypes.generated.h"

UENUM(BlueprintType)
enum class ERPGDamageZone : uint8
{
	ENormal = 0          UMETA(DisplayName = "Normal Zone"),
	EImmune = 1	         UMETA(DisplayName = "Zero Damage Zone"),
	EHighDefense = 2	 UMETA(DisplayName = "High Defense Zone"),
	EHighDamage = 3      UMETA(DisplayName = "High Damage Zone"),
};

USTRUCT(BlueprintType)
struct ACTIONRPG_API FRPGDamageEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient, BlueprintReadOnly)
	FName ContextString;

	UPROPERTY(Transient, BlueprintReadOnly)
	AController* InstigatorController = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly)
	// Actor that deals damage
	AActor* InstigatorActor = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly)
	// Actor that is received damage
	AActor* TargetActor = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly)
	float Damage = 0.f;

	UPROPERTY(Transient, BlueprintReadOnly)
	bool bIsFinalBlow = false;
};

USTRUCT(BlueprintType)
struct ACTIONRPG_API FRPGEntityDeathDelegateData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FRPGDamageEvent DamageEvent;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGEntityDeathDelegate, const FRPGEntityDeathDelegateData&, deathDelegateData);

DECLARE_MULTICAST_DELEGATE_OneParam(FRPGEntityDeathNativeDelegate, const FRPGEntityDeathDelegateData&);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGHealthChangedDelegate, float, newHealth, float, oldHealth);

DECLARE_MULTICAST_DELEGATE_TwoParams(FRPGHealthChangedNativeDelegate, float, float);