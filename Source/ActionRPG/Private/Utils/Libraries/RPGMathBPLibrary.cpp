// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/Libraries/RPGMathBPLibrary.h"
#include "Interfaces/RPGMovableInterface.h"
#include "GameFramework/MovementComponent.h"

FVector URPGMathBPLibrary::PredictTargetLocationOutTime(const float callerSpeed, const FVector& callerLocation,
                                                        const AActor* targetActor, const float clampTargetVelocity, float& outTimeNeededToReachTarget)
{
	FVector predictedLocation = FVector::ZeroVector;
	if (targetActor || callerSpeed == 0.f)
	{
		return predictedLocation;
	}

	outTimeNeededToReachTarget = CalculateTimeToReachLocation(callerSpeed, callerLocation, targetActor->GetActorLocation());

	const FVector& targetVelocity = (clampTargetVelocity == 0.f) ?
									targetActor->GetVelocity() :
									targetActor->GetVelocity().GetClampedToMaxSize(clampTargetVelocity);
	const FVector& targetLocation = targetActor->GetActorLocation();

	predictedLocation = targetLocation + (targetVelocity * outTimeNeededToReachTarget);

	outTimeNeededToReachTarget = CalculateTimeToReachLocation(callerSpeed, callerLocation, predictedLocation);

	return predictedLocation;
}

FVector URPGMathBPLibrary::PredictTargetLocationInTime(const float time, const AActor* targetActor,
	const float clampTargetVelocity)
{
	FVector predictedLocation = FVector::ZeroVector;

	if (!targetActor)
	{
		return predictedLocation;
	}

	const FVector& targetVelocity = (clampTargetVelocity == 0.f) ?
									targetActor->GetVelocity() :
									targetActor->GetVelocity().GetClampedToMaxSize(clampTargetVelocity);

	return predictedLocation = targetActor->GetActorLocation() + (targetVelocity * time);
}

FVector URPGMathBPLibrary::PredictTargetLocation(const AActor* callerActor, const AActor* targetActor,
	const float maxReachTimeToConsider)
{
	if (!callerActor || !targetActor)
	{
		return FVector::ZeroVector;
	}

	const FVector& callerLocation = callerActor->GetActorLocation();
	const FVector& targetLocation = targetActor->GetActorLocation();
	const IRPGMovableInterface* movableCaller = Cast<const IRPGMovableInterface>(callerActor);
	const IRPGMovableInterface* movableTarget = Cast<const IRPGMovableInterface>(targetActor);
	if (!movableCaller || !movableTarget)
	{
		return targetLocation;
	}

	const UMovementComponent* callerMovementComp = movableCaller->GetBaseMovementComponent();
	const UMovementComponent* targetMovementComp = movableTarget->GetBaseMovementComponent();
	if (!callerMovementComp || !targetMovementComp)
	{
		return targetLocation;
	}

	const float callerSpeed = callerMovementComp->Velocity.Size();
	const float timeToReach = (callerSpeed < KINDA_SMALL_NUMBER) ? 
							  maxReachTimeToConsider :
							  FMath::Min(CalcDistanceBetweenVectors(callerLocation, targetLocation) / callerSpeed, maxReachTimeToConsider);
	
	return targetLocation + targetMovementComp->Velocity * timeToReach;
}

float URPGMathBPLibrary::CalculateTimeToReachLocation(float callerSpeed, const FVector& callerLocation,
	const FVector& targetLocation)
{
	const float distanceToTarget = (callerLocation - targetLocation).Size();
	return distanceToTarget / callerSpeed;
}

float URPGMathBPLibrary::CalcAngleBetweenVectors(const FVector& vectorA, const FVector& vectorB)
{
	if (vectorA.IsZero() || vectorB.IsZero())
	{
		return 0.f;
	}

	return FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(vectorA, vectorB) / (vectorA.Size() * vectorB.Size())));
}

float URPGMathBPLibrary::CalcAngleBetweenNormalizedVectors(const FVector& vectorA, const FVector& vectorB)
{
	return FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(vectorA, vectorB)));
}

float URPGMathBPLibrary::CalcDistanceBetweenVectors(const FVector& vectorA, const FVector& vectorB)
{
	return FVector::Distance(vectorA, vectorB);
}

float URPGMathBPLibrary::CalcSqrtDistanceBetweenVectors(const FVector& vectorA, const FVector& vectorB)
{
	return FVector::DistSquared(vectorA, vectorB);
}

float URPGMathBPLibrary::CalcDirection(const FVector& velocity, const FRotator& baseRotation)
{
	if (velocity.IsNearlyZero())
	{
		const FMatrix rotMatrix = FRotationMatrix(baseRotation);
		const FVector forwardVector = rotMatrix.GetScaledAxis(EAxis::X);
		const FVector rightVector = rotMatrix.GetScaledAxis(EAxis::Y);
		const FVector normalizedVelocity = velocity.GetSafeNormal2D();

		// Get a cos(alpha) of forward vector vs velocity
		const float forwardCosAngle = FVector::DotProduct(forwardVector, normalizedVelocity);

		// Get the alpha and convert to degree
		float fowardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(forwardCosAngle));

		// Depending on where right vector is, flip it
		const float rightCosAngle = FVector::DotProduct(rightVector, normalizedVelocity);
		if (rightCosAngle < 0.f)
		{
			fowardDeltaDegree *= -1.f;
		}

		return fowardDeltaDegree;
	}

	return 0.f;
}

ERPGDirection URPGMathBPLibrary::GetRelativeDirectionsBetweenActors(const AActor* actorA, const AActor* actorB)
{
	if (!actorA || !actorB)
	{
		return ERPGDirection::EBack;
	}

	const FVector relativeDirection = actorA->GetActorLocation() - actorB->GetActorLocation();
	const FVector normalRelativeDirection = relativeDirection.GetUnsafeNormal();

	const float forwardDotProduct = FVector::DotProduct(actorA->GetActorForwardVector(), normalRelativeDirection);
	const float rightDotProduct = FVector::DotProduct(actorB->GetActorRightVector(), normalRelativeDirection);

	if (forwardDotProduct >= 0.707f)
	{
		return ERPGDirection::EFront;
	}
	else if (rightDotProduct >= 0.707f)
	{
		return ERPGDirection::ERight;
	}
	else if (rightDotProduct <= -0.707f)
	{
		return ERPGDirection::ELeft;
	}
	else
	{
		return ERPGDirection::EBack;
	}
}
