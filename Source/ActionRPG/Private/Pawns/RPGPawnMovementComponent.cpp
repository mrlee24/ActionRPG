// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/RPGPawnMovementComponent.h"

#include "Interfaces/RPGMovableInterface.h"
#include "Utils/Libraries/RPGGameplayBPLibrary.h"

DEFINE_LOG_CATEGORY(RPGPawnMovementComponentLog);
DECLARE_STATS_GROUP(TEXT("RPGPawnMovementComponent"), STATGROUP_RPGPawnMovementComponent, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("RPGPawnMovementComponent - CalcAcceleration"), STAT_RPGPawnMovementComponentCalcAcceleration,
STATGROUP_RPGPawnMovementComponent);
DECLARE_CYCLE_STAT(TEXT("RPGPawnMovementComponent - CalcVelocity"), STAT_RPGPawnMovementComponentCalcVelocity,
STATGROUP_RPGPawnMovementComponent);
DECLARE_CYCLE_STAT(TEXT("RPGPawnMovementComponent - UpdatePawnRotation"), STAT_RPGPawnMovementComponentUpdatePawnRotation,
STATGROUP_RPGPawnMovementComponent);
DECLARE_CYCLE_STAT(TEXT("RPGPawnMovementComponent - FindFloorAndSetMovementMode"), STAT_RPGPawnMovementComponentFindFloorAndSetMovementMode,
STATGROUP_RPGPawnMovementComponent);
DECLARE_CYCLE_STAT(TEXT("RPGPawnMovementComponent - ApplyFallingMovement"), STAT_RPGPawnMovementComponentApplyFallingMovement,
STATGROUP_RPGPawnMovementComponent);
DECLARE_CYCLE_STAT(TEXT("RPGPawnMovementComponent - MoveAlongFloor"), STAT_RPGPawnMovementComponentMoveAlongFloor,
STATGROUP_RPGPawnMovementComponent);
DECLARE_CYCLE_STAT(TEXT("RPGPawnMovementComponent - ApplySpacialMovement"), STAT_RPGPawnMovementComponentApplySpacialMovement,
STATGROUP_RPGPawnMovementComponent);

CSV_DEFINE_CATEGORY(RPGPawnMovementComponentMS, true);

URPGPawnMovementComponent::URPGPawnMovementComponent(const FObjectInitializer& objectInitializer)
{	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	DefaultMovementMode = EMovementMode::MOVE_Walking;
	GravityScale = 1.f;
	MaxFallSpeed = 800.f;
	MaxWalkSpeed = 300.f;
	MaxAcceleration = 50.f;
	MaxWalkHeight = 10.f;
	WalkHeightTolerance = 5.0f;
	BrakingDecelerationWalking = 200.f;
	BrakingDecelerationFalling = 200.f;
	WalkableFloorAngle = 50.f;
	bMaintainHorizontalGroundVelocity = true;
	RotationRate = FRotator(0.f, 100.f, 0.f);
	bOrientRotationToMovement = true;
	bOnlyMoveForward = true;
	bRequestedMoveUseAcceleration = true;
}

void URPGPawnMovementComponent::SetUpdatedComponent(USceneComponent* newUpdatedComponent)
{
	if (newUpdatedComponent)
	{
		if (!ensureMsgf(Cast<APawn>(newUpdatedComponent->GetOwner()),
			TEXT("%s must update a component owned by LSPawn"),
			ANSI_TO_TCHAR(__func__)))
		{
			return;
		}
	}
	
	Super::SetUpdatedComponent(newUpdatedComponent);

	OwnerPawn = UpdatedComponent ? CastChecked<APawn>(UpdatedComponent->GetOwner()) : nullptr;
	if (OwnerPawn)
	{
		const IRPGMovableInterface* movableInterface = Cast<IRPGMovableInterface>(OwnerPawn);
		if (movableInterface)
		{
			FallTraceDistance = movableInterface->GetScaledCapsuleHalfHeight() + MaxWalkHeight;
		}
	}
}

float URPGPawnMovementComponent::GetMaxSpeed() const
{
	if (!URPGGameplayBPLibrary::IsRunningGameplayWorld(GetWorld()))
	{
		return Super::GetMaxSpeed();
	}

	switch (MovementMode)
	{
	case MOVE_Walking:
		return CurrentLocomotionState.MaxSpeed;
	case MOVE_NavWalking:
		return CurrentLocomotionState.MaxSpeed;
	case MOVE_Falling:
		return 600.f;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return CurrentLocomotionState.MaxSpeed;
	case MOVE_None:
	default:
		return 0.f;
	}
}

float URPGPawnMovementComponent::GetGravityZ() const
{
	return Super::GetGravityZ() * GravityScale;
}

void URPGPawnMovementComponent::StopActiveMovement()
{
	Super::StopActiveMovement();

	Acceleration = FVector::ZeroVector;
	bHasRequestedVelocity = false;
	RequestedVelocity = FVector::ZeroVector;
	Velocity = FVector::ZeroVector;
}

void URPGPawnMovementComponent::RequestDirectMove(const FVector& moveVelocity, bool forceMaxSpeed)
{
	if (moveVelocity.SizeSquared() < KINDA_SMALL_NUMBER)
	{
		return;
	}

	RequestedVelocity = moveVelocity;

	// Zero out Z component if the pawn is walking or falling
	if (IsWalkingOrFalling())
	{
		RequestedVelocity.Z = 0.0f;
	}

	const float requestedSpeed = RequestedVelocity.Size();
	const FVector requestedNormalized = RequestedVelocity / requestedSpeed;
	RequestedVelocity = (forceMaxSpeed) ?
		requestedNormalized * GetMaxSpeed() :
		requestedNormalized * FMath::Min(GetMaxSpeed(), requestedSpeed);

	bHasRequestedVelocity = true;
}

void URPGPawnMovementComponent::RequestPathMove(const FVector& moveInput)
{
	Super::RequestPathMove(moveInput);
}

bool URPGPawnMovementComponent::CanStartPathFollowing() const
{
	return Super::CanStartPathFollowing();
}

bool URPGPawnMovementComponent::CanStopPathFollowing() const
{
	return Super::CanStopPathFollowing();
}

void URPGPawnMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Cache values
	WalkableFloorZ = FMath::Cos(FMath::DegreesToRadians(WalkableFloorAngle));
}

void URPGPawnMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	MovementMode = DefaultMovementMode;
	FindFloorAndSetMovementMode();
}

void URPGPawnMovementComponent::TickComponent(float deltaTime, enum ELevelTick tickType,
	FActorComponentTickFunction* thisTickFunction)
{
	if (!HasValidData() || ShouldSkipUpdate(deltaTime))
	{
		return;
	}

	const FVector inputVector = OwnerPawn->Internal_ConsumeMovementInputVector();
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	// Super tick may destroy/invalidate PawnOwner or UpdatedComponent, so we need to re-check.
	if (!HasValidData())
	{
		return;
	}

	// Don't update if fell out of the world
	if (!OwnerPawn->CheckStillInWorld())
	{
		return;
	}

	// Don't update if simulating physics.
	if (UpdatedComponent->IsSimulatingPhysics())
	{
		return;
	}

	// If movement mode is Walking and current floor trace result does not block
	// Enter falling state
	if (MovementMode == EMovementMode::MOVE_Walking && !CurrentFloorTraceResult.bBlockingHit)
	{
		MovementMode = EMovementMode::MOVE_Falling;
	}

	// If falling, apply vertical movement and keep initial lateral velocity
	if (MovementMode == EMovementMode::MOVE_Falling)
	{
		ApplyFallingMovement(deltaTime);
		FindFloorAndSetMovementMode();
		return;
	}

	// Compute acceleration, rotation and velocity
	CalcAcceleration(inputVector, deltaTime);
	UpdatePawnRotation(deltaTime);
	CalcVelocity(deltaTime);

	if (!Velocity.IsZero())
	{
		if (MovementMode == EMovementMode::MOVE_Walking)
		{
			MoveAlongFloor(deltaTime);
			FindFloorAndSetMovementMode();
		}
		else if (MovementMode == EMovementMode::MOVE_Flying || MovementMode == EMovementMode::MOVE_Swimming)
		{
			ApplySpacialMovement(deltaTime * Velocity);
		}
	}

	// Consume the requested velocity.
	bHasRequestedVelocity = false;
}

APawn* URPGPawnMovementComponent::GetOwnerPawn() const
{
	return OwnerPawn;
}

bool URPGPawnMovementComponent::HasValidData() const
{
	return IsValid(OwnerPawn) && UpdatedComponent;
}

FVector URPGPawnMovementComponent::GetCurrentAcceleration() const
{
	return Acceleration;
}

void URPGPawnMovementComponent::SetMaxWalkSpeed(const float speed)
{
	MaxWalkSpeed = speed;
}

void URPGPawnMovementComponent::SetMaxAcceleration(const float acceleration)
{
	MaxAcceleration = acceleration;
}

void URPGPawnMovementComponent::AddInputVector(const FVector& moveInput)
{
	if (OwnerPawn)
	{
		OwnerPawn->Internal_AddMovementInput(moveInput);
	}
}

FRotator URPGPawnMovementComponent::GetRotationRate() const
{
	return RotationRate;
}

bool URPGPawnMovementComponent::IsFalling() const
{
	return MovementMode == MOVE_Falling;
}

void URPGPawnMovementComponent::CalcAcceleration(const FVector& inputVector, float deltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_RPGPawnMovementComponentCalcAcceleration);
	CSV_SCOPED_TIMING_STAT(RPGPawnMovementComponentMS, STAT_RPGPawnMovementComponentCalcAcceleration);
	
	if (bHasRequestedVelocity)
	{
		if (bRequestedMoveUseAcceleration)
		{
			// Update the Acceleration aim for the requested velocity
			Acceleration = (RequestedVelocity - Velocity) / deltaTime;
			Acceleration = Acceleration.GetClampedToMaxSize(MaxAcceleration);
			return;
		}

		// Requested Velocity will be used directly, ignore acceleration
		Acceleration = FVector::ZeroVector;
		return;
	}

	FVector adjustedInputVector(inputVector);

	// If pawn is walking or falling, ignore Z axis input vector
	if (IsWalkingOrFalling())
	{
		adjustedInputVector.Z = 0.f;
	}

	// Clamp size to 0~1 and scale with MaxAcceleration
	Acceleration = MaxAcceleration * adjustedInputVector.GetClampedToMaxSize(1.0f);
}

void URPGPawnMovementComponent::CalcVelocity(float deltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_RPGPawnMovementComponentCalcVelocity);
	CSV_SCOPED_TIMING_STAT(RPGPawnMovementComponentMS, STAT_RPGPawnMovementComponentCalcVelocity);

	if (bHasRequestedVelocity && !bRequestedMoveUseAcceleration)
	{
		// Set the velocity directly and ignore acceleration
		Velocity = RequestedVelocity;
		return;
	}

	const float currentSpeedSqr = Velocity.SizeSquared();
	const FVector forwardDir = OwnerPawn->GetActorForwardVector();
	float currentSpeed = -1.f;

	// If only move forward mode is on and speed is not zero, orient the current velocity towards pawn forward vector
	if (bOnlyMoveForward && bOrientRotationToMovement && currentSpeedSqr > 0)
	{
		currentSpeed = FMath::Sqrt(currentSpeedSqr);
		Velocity = forwardDir * currentSpeed;
	}

	// If no acceleration and speed is greater than 0, or speed is greater than max speed, apply brake. Otherwise apply acceleration
	// Make sure brake should not change the velocity's direction, but can set it to zero
	if ((Acceleration.IsZero() && currentSpeedSqr > 0) || currentSpeedSqr > FMath::Square(GetMaxSpeed()))
	{
		currentSpeed = currentSpeed < 0 ? FMath::Sqrt(currentSpeedSqr) : currentSpeed;
		const float newSpeed = FMath::Max(0.f, currentSpeed - BrakingDecelerationWalking * deltaTime);
		Velocity = Velocity / currentSpeed * newSpeed;
	}
	else if (bOnlyMoveForward && bOrientRotationToMovement)
	{
		// If current acceleration is not zero, add the amount of acceleration to the current velocity at the direction of pawn's forward vector
		if (!Acceleration.IsZero())
		{
			const float accelerationSize = Acceleration.Size();
			Velocity += forwardDir * accelerationSize * deltaTime;
		}
	}
	else
	{
		Velocity += Acceleration * deltaTime;
	}
}

void URPGPawnMovementComponent::UpdatePawnRotation(float deltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_RPGPawnMovementComponentUpdatePawnRotation);
	CSV_SCOPED_TIMING_STAT(RPGPawnMovementComponentMS, STAT_RPGPawnMovementComponentUpdatePawnRotation);

	if (!bOrientRotationToMovement)
	{
		return;
	}

	FRotator currentRotation = UpdatedComponent->GetComponentRotation();
	FRotator deltaRot = FRotator(RotationRate.Pitch * deltaTime, RotationRate.Yaw * deltaTime, RotationRate.Roll * deltaTime);
	FRotator desiredRotation = currentRotation;

	// If AI path following requested a velocity, and we are not using acceleration, use requested velocity's direction as desiredRotation.
	if (bHasRequestedVelocity && !bRequestedMoveUseAcceleration && RequestedVelocity.SizeSquared() > KINDA_SMALL_NUMBER)
	{
		desiredRotation = RequestedVelocity.GetSafeNormal().Rotation();
	}
	else if (Acceleration.SizeSquared() > KINDA_SMALL_NUMBER)
	{
		// Rotate toward direction of acceleration.
		// Don't update rotation if acceleration is 0 or very small.
		desiredRotation = Acceleration.GetSafeNormal().Rotation();
	}

	// Restrict desiredRotation to only Yaw component for walking and falling pawns
	if (IsWalkingOrFalling())
	{
		desiredRotation.Pitch = 0.f;
		desiredRotation.Yaw = FRotator::NormalizeAxis(desiredRotation.Yaw);
		desiredRotation.Roll = 0.f;
	}
	else
	{
		desiredRotation.Normalize();
	}

	// Compute desiredRotation for this tick
	if (!currentRotation.Equals(desiredRotation, KINDA_SMALL_NUMBER))
	{
		// Pitch
		if (!FMath::IsNearlyEqual(currentRotation.Pitch, desiredRotation.Pitch, KINDA_SMALL_NUMBER))
		{
			desiredRotation.Pitch = FMath::FixedTurn(currentRotation.Pitch, desiredRotation.Pitch, deltaRot.Pitch);
		}

		// Yaw
		if (!FMath::IsNearlyEqual(currentRotation.Yaw, desiredRotation.Yaw, KINDA_SMALL_NUMBER))
		{
			desiredRotation.Yaw = FMath::FixedTurn(currentRotation.Yaw, desiredRotation.Yaw, deltaRot.Yaw);
		}

		// Roll
		if (!FMath::IsNearlyEqual(currentRotation.Roll, desiredRotation.Roll, KINDA_SMALL_NUMBER))
		{
			desiredRotation.Roll = FMath::FixedTurn(currentRotation.Roll, desiredRotation.Roll, deltaRot.Roll);
		}
	}

	OwnerPawn->SetActorRotation(desiredRotation);
}

bool URPGPawnMovementComponent::IsWalkingOrFalling() const
{
	return MovementMode == MOVE_Walking || MovementMode == MOVE_NavWalking || MovementMode == MOVE_Falling;
}

void URPGPawnMovementComponent::FindFloorAndSetMovementMode()
{
	SCOPE_CYCLE_COUNTER(STAT_RPGPawnMovementComponentFindFloorAndSetMovementMode);
	CSV_SCOPED_TIMING_STAT(RPGPawnMovementComponentMS, STAT_RPGPawnMovementComponentFindFloorAndSetMovementMode);

	if (OwnerPawn && (MovementMode == EMovementMode::MOVE_Walking || MovementMode == EMovementMode::MOVE_Falling))
	{
		const FVector start = OwnerPawn->GetActorLocation();
		const FVector end = start + FVector(0.f, 0.f, -FallTraceDistance);
		FCollisionQueryParams queryParams;
		bool blockHit = GetWorld()->LineTraceSingleByChannel(CurrentFloorTraceResult, start, end, ECollisionChannel::ECC_WorldDynamic, queryParams);
		if (blockHit)
		{
			MovementMode = EMovementMode::MOVE_Walking;
			FallSpeed = 0;

			// Adjust the pawn walking height if necessary
			const float deltaWalkHeight = FallTraceDistance - (start.Z - CurrentFloorTraceResult.ImpactPoint.Z);
			if (deltaWalkHeight > WalkHeightTolerance)
			{
				FHitResult hit;
				SafeMoveUpdatedComponent(FVector(0.f, 0.f, deltaWalkHeight), UpdatedComponent->GetComponentQuat(), false, hit);
			}
		}
		else
		{
			MovementMode = EMovementMode::MOVE_Falling;
		}
	}
}

void URPGPawnMovementComponent::ApplyFallingMovement(float deltaSeconds)
{
	SCOPE_CYCLE_COUNTER(STAT_RPGPawnMovementComponentApplyFallingMovement);
	CSV_SCOPED_TIMING_STAT(RPGPawnMovementComponentMS, STAT_RPGPawnMovementComponentApplyFallingMovement);
	// Apply lateral deceleration
	if (Velocity.X != 0 || Velocity.Y != 0)
	{
		const float currentLateralSpeed = Velocity.Size2D();
		const float newLateralSpeed = FMath::Max(0.f, currentLateralSpeed - BrakingDecelerationFalling * deltaSeconds);
		Velocity = Velocity / currentLateralSpeed * newLateralSpeed;
	}

	FallSpeed = FMath::Max(MaxFallSpeed, FallSpeed + GetGravityZ() * deltaSeconds);
	const FVector delta = FVector(Velocity.X, Velocity.Y, -FallSpeed) * deltaSeconds;
	FHitResult hit(1.f);
	SafeMoveUpdatedComponent(delta, UpdatedComponent->GetComponentQuat(), true, hit);

	if (hit.bStartPenetrating || hit.IsValidBlockingHit())
	{
		HandleBlockingHit(hit, delta);
	}
}

void URPGPawnMovementComponent::MoveAlongFloor(float deltaSeconds)
{
	SCOPE_CYCLE_COUNTER(STAT_RPGPawnMovementComponentMoveAlongFloor);
	CSV_SCOPED_TIMING_STAT(RPGPawnMovementComponentMS, STAT_RPGPawnMovementComponentMoveAlongFloor);
	// Move along the current floor
	const FVector delta = FVector(Velocity.X, Velocity.Y, 0.f) * deltaSeconds;
	const FVector adjustedDelta = CancelRampUpMovementIfNotWalkable(delta, CurrentFloorTraceResult);

	FHitResult hit(1.f);
	FVector rampVector = ComputeRampMoveDelta(adjustedDelta, CurrentFloorTraceResult);
	SafeMoveUpdatedComponent(rampVector, UpdatedComponent->GetComponentQuat(), true, hit);
	float lastMoveTimeSlice = deltaSeconds;

	if (hit.bStartPenetrating)
	{
		HandleBlockingHit(hit, adjustedDelta);
	}
	else if (hit.IsValidBlockingHit())
	{
		// We impacted something (most likely another ramp, but possibly a barrier).
		float percentTimeApplied = hit.Time;
		if ((hit.Time > 0.f) && (hit.Normal.Z > KINDA_SMALL_NUMBER) && IsWalkable(hit))
		{
			// Another walkable ramp.
			const float initialPercentRemaining = 1.f - percentTimeApplied;
			rampVector = ComputeRampMoveDelta(adjustedDelta * initialPercentRemaining, hit);
			lastMoveTimeSlice = initialPercentRemaining * lastMoveTimeSlice;
			SafeMoveUpdatedComponent(rampVector, UpdatedComponent->GetComponentQuat(), true, hit);

			const float secondHitPercent = hit.Time * initialPercentRemaining;
			percentTimeApplied = FMath::Clamp(percentTimeApplied + secondHitPercent, 0.f, 1.f);
		}

		// Currently not support Towers pawn step up, so just slide along surface.
		if (hit.IsValidBlockingHit())
		{
			HandleImpact(hit, lastMoveTimeSlice, rampVector);
			SlideAlongSurface(adjustedDelta, 1.f - percentTimeApplied, hit.Normal, hit, true);
		}
	}
}

void URPGPawnMovementComponent::ApplySpacialMovement(const FVector& delta)
{
	SCOPE_CYCLE_COUNTER(STAT_RPGPawnMovementComponentApplySpacialMovement);
	CSV_SCOPED_TIMING_STAT(RPGPawnMovementComponentMS, STAT_RPGPawnMovementComponentApplySpacialMovement);
	FHitResult hit(1.f);
	SafeMoveUpdatedComponent(delta, UpdatedComponent->GetComponentQuat(), true, hit);

	if (hit.bStartPenetrating || hit.IsValidBlockingHit())
	{
		HandleBlockingHit(hit, delta);
	}
}

FVector URPGPawnMovementComponent::ComputeRampMoveDelta(const FVector& delta, const FHitResult& rampHit) const
{
	const float floorNormalZ = rampHit.ImpactNormal.Z;
	if (floorNormalZ < 1.f - KINDA_SMALL_NUMBER && floorNormalZ > KINDA_SMALL_NUMBER)
	{
		const float floorDotDelta = rampHit.ImpactNormal | delta;
		FVector rampMovement(delta.X, delta.Y, -floorDotDelta / floorNormalZ);

		if (bMaintainHorizontalGroundVelocity)
		{
			return rampMovement;
		}
		else
		{
			return rampMovement.GetSafeNormal() * delta.Size();
		}
	}

	return delta;
}

bool URPGPawnMovementComponent::IsWalkable(const FHitResult& hit) const
{
	return hit.IsValidBlockingHit() && hit.ImpactNormal.Z >= WalkableFloorZ;
}

void URPGPawnMovementComponent::HandleBlockingHit(FHitResult& hit, const FVector& delta)
{
	HandleImpact(hit);
	SlideAlongSurface(delta, 1.f, hit.Normal, hit, true);

	if (hit.bStartPenetrating)
	{
		UE_LOG(RPGPawnMovementComponentLog, Verbose, TEXT("Pawn stucks in geometry"));
	}
}

FVector URPGPawnMovementComponent::CancelRampUpMovementIfNotWalkable(const FVector& delta, const FHitResult& hit)
{
	if (IsWalkable(hit))
	{
		return delta;
	}

	const FVector normalXY = FVector(hit.ImpactNormal.X, hit.ImpactNormal.Y, 0.f).GetSafeNormal2D(KINDA_SMALL_NUMBER);
	const float dotNormalXYDelta = normalXY | delta;

	// If move downwards the ramp, don't adjust the delta
	if (dotNormalXYDelta > 0)
	{
		return delta;
	}

	// If move upwards the ramp, cancel the part that move against the ramp normal XY direction
	return delta - dotNormalXYDelta * normalXY;
}

