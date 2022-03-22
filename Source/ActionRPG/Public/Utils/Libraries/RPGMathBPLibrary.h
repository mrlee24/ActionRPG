// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RPGMathBPLibrary.generated.h"

UENUM(BlueprintType)
enum class ERPGDirection : uint8
{
	EFront = 0      UMETA(DisplayName = "Front"),
	EBack = 1       UMETA(DisplayName = "Back"),
	ELeft = 2       UMETA(DisplayName = "Left"),
	ERight = 3      UMETA(DisplayName = "Right")
};

/**
 * 
 */
UCLASS()
class ACTIONRPG_API URPGMathBPLibrary : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Utils | Math")
	/**
	* Calculates predicted location of given actor based on his velocity and given caller speed
	* @param callerSpeed - Speed of caller (usually velocity length)
	* @param target - Actor of whom to predict location
	* @param outTimeNeededToReachLoc - Time needed to reach predicted location
	* @param clampTargetVel - Whether to use clamped velocity of target when predicting, 0 means don't clamp
	* @return Predicted Location (where target will be)
	*/
	static FVector PredictTargetLocationOutTime(const float callerSpeed, const FVector& callerLocation, const AActor* targetActor, 
		                                        const float clampTargetVelocity, float& outTimeNeededToReachTarget);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Utils | Math")
	/**
	* Calculates predicted location of given actor based on his velocity and given caller speed
	* @param time
	* @param target - Actor of whom to predict location
	* @param clampTargetVel - Whether to use clamped velocity of target when predicting, 0 means don't clamp
	* @return Predicted Location (where target will be)
	*/
	static FVector PredictTargetLocationInTime(const float time, const AActor* targetActor, const float clampTargetVelocity = 0.f);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Utils | Math")
	/**
	* Simple estimate the target's future location when caller reaches it
	* Note that both actors must implement TowersMovableInterface
	* @param caller - Actor who is moving towards the target
	* @param target - Actor of whom to predict location
	* @param maxReachTimeToConsider - Since the longer it takes the caller to reach the target, the less accurate this estimate would be
	* If it takes longer to reach the target, use this value instead.
	*/
	static FVector PredictTargetLocation(const AActor* callerActor, const AActor* targetActor, const float maxReachTimeToConsider = 5.0);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Utils | Math")
	/**
	* Calculates time needed to reach given location
	* @param callerSpeed - Speed of caller (usually velocity length)
	* @return Predicted Location (where target will be)
	*/
	static float CalculateTimeToReachLocation(float callerSpeed, const FVector& callerLocation,
			const FVector& targetLocation);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Utils | Math")
	// Compute the angle between two vectors vectorA and vectorB. Result is in degree.
	static float CalcAngleBetweenVectors(const FVector& vectorA, const FVector& vectorB);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Utils | Math")
	// Compute the angle between two normalized vectors vectorA and vectorB. Result is in degree.
	static float CalcAngleBetweenNormalizedVectors(const FVector& vectorA, const FVector& vectorB);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Utils | Math")
	// Compute the distance between two vectors vectorA and vectorB.
	static float CalcDistanceBetweenVectors(const FVector& vectorA, const FVector& vectorB);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Utils | Math")
	// Compute the distance between two vectors vectorA and vectorB. Result is in squared value.
	static float CalcSqrtDistanceBetweenVectors(const FVector& vectorA, const FVector& vectorB);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Utils | Math")
	// Compute angle that we are facing at
	static float CalcDirection(const FVector& velocity, const FRotator& baseRotation);

public:
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Utils | Math")
	// Compute relative direction between two Actors 
	static ERPGDirection GetRelativeDirectionsBetweenActors(const AActor* actorA, const AActor* actorB);
};
