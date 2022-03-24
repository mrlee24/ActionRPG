// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/RPGAnimationTypes.h"
#include "GameFramework/NavMovementComponent.h"
#include "RPGPawnMovementComponent.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(RPGPawnMovementComponentLog, Log, All);

CSV_DECLARE_CATEGORY_EXTERN(RPGPawnMovementComponent);

UCLASS(ClassGroup=(ActionRPG), meta=(BlueprintSpawnableComponent))
class ACTIONRPG_API URPGPawnMovementComponent : public UNavMovementComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGPawnMovementComponent(const FObjectInitializer& objectInitializer);

public: // UMovementComponent Interface
	virtual void SetUpdatedComponent(USceneComponent* newUpdatedComponent) override;
	virtual float GetMaxSpeed() const override;
	virtual float GetGravityZ() const override;
	virtual void StopActiveMovement() override;
	virtual void RequestDirectMove(const FVector& moveVelocity, bool forceMaxSpeed) override;
	virtual void RequestPathMove(const FVector& moveInput) override;
	virtual bool CanStartPathFollowing() const override;
	virtual bool CanStopPathFollowing() const override;

protected: // UActorComponent
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;

public:
	// Return the Pawn that owns UpdatedComponent.
	UFUNCTION(BlueprintCallable, Category = "RPGPawnMovementComponent")
	class APawn* GetOwnerPawn() const;

	// return true if OwnerPawn is not null and not pending kill, and UpdatedComponent is not null.
	virtual bool HasValidData() const;

	UFUNCTION(BlueprintPure, Category = "RPGPawnMovementComponent")
	FVector GetCurrentAcceleration() const;

	void SetMaxWalkSpeed(const float speed);
	void SetMaxAcceleration(const float acceleration);

	// Add movement input, does not ignore Z input
	void AddInputVector(const FVector& moveInput);

	FRotator GetRotationRate() const;

	bool IsFalling() const;

private:
	// Calculate the acceleration for this tick
	void CalcAcceleration(const FVector& inputVector, float deltaTime);

	// Calculate the Velocity based on the acceleration and deceleration. May apply velocity requested by path following component
	void CalcVelocity(float deltaTime);

	// If bOrientRotationToMovement is true, update the the rotation of the pawn towards the movement using RotationRate
	void UpdatePawnRotation(float deltaTime);

	bool IsWalkingOrFalling() const;

	// Single line trace downward from the pawn against floor with the distance of MaxWalkHeight
	// If does not hit, set movement mode to falling
	// Otherwise set move mode to walking and fill CurrentFloorTraceResult
	void FindFloorAndSetMovementMode();

	// Location update for falling pawns
	void ApplyFallingMovement(float deltaSeconds);

	// Adapted from CharacterMovementComponent::MoveAlongFloor
	void MoveAlongFloor(float deltaSeconds);

	// Location update for flying or swimming pawns
	void ApplySpacialMovement(const FVector& delta);

	// Compute a vector that moves parallel to the surface, by projecting the horizontal movement direction onto the ramp.
	FVector ComputeRampMoveDelta(const FVector& delta, const FHitResult& rampHit) const;

	bool IsWalkable(const FHitResult& hit) const;

	void HandleBlockingHit(FHitResult& hit, const FVector& delta);

	FVector CancelRampUpMovementIfNotWalkable(const FVector& delta, const FHitResult& hit);
	
protected:
	// Pawn owns this component.
	UPROPERTY(Transient, DuplicateTransient)
	class APawn* OwnerPawn = nullptr;

public:
	// The default movement mode of the pawn.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPGPawnMovementComponent")
	TEnumAsByte<enum EMovementMode> DefaultMovementMode;

	// Custom gravity scale. Gravity is multiplied by this amount for the pawn.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPGPawnMovementComponent")
	float GravityScale;

	// This represents the lowest point of the walking pawn's capsule to the ground.
	// Pawn will enter falling state if the actual distance is bigger than this value.
	// Increasing this value (and shrink the capsule height) helps stepping over low obstacles.
	// Increasing this value can also help climbing more steep ramps (to prevent capsule bouncing off).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"), Category = "RPGPawnMovementComponent")
	float MaxWalkHeight;

	// The maximum amount of offset we allow in the pawn's walk height.
	// If the pawn walks too low, its Z position will be adjusted.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "0"), Category = "RPGPawnMovementComponent")
	float WalkHeightTolerance;

	// For now supports walking and falling
	UPROPERTY(BlueprintReadOnly, Category = "RPGPawnMovementComponent")
	TEnumAsByte<enum EMovementMode> MovementMode;

	// Deceleration when walking and not applying acceleration. This is a constant opposing force that directly lowers velocity by a constant value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"), Category = "RPGPawnMovementComponent")
	float BrakingDecelerationWalking;

	// Lateral deceleration when falling and not applying acceleration.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"), Category = "RPGPawnMovementComponent")
	float BrakingDecelerationFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPGPawnMovementComponent")
	// If true, will keep same speed on ramps as on the horizontal floor.
	// If false, the horizontal value of velocity (X and Y) will be the same.
	uint8 bMaintainHorizontalGroundVelocity : 1;

	// Change in rotation per second, used when OrientRotationToMovement are true. Set a negative value for infinite rotation rate and instant turns.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPGPawnMovementComponent: Rotation")
	FRotator RotationRate;

	// If true, rotate the Character toward the direction of acceleration, using RotationRate as the rate of rotation change.
	// Sometimes we may want the pawn to not rotate toward the direction of acceleration, e.g. enemy circle around the player.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPGPawnMovementComponent: Rotation")
	uint8 bOrientRotationToMovement : 1;

	// If true, pawn will first update rotation by acceleration, then move FORWARD. (acceleration does not change velocity direction, only rotation changes it).
	// In this mode, pawn will not slide when turning, and make a "U turn" when changing direction
	// this mode is suitable for walking pawns like animals who cannot move sideway.
	// This is ignored if bOrientRotationToMovement is set to false.
	// Warning: if turn speed is too low compare to walk speed, pawn might never reach the target location.
	// If false, pawn will update location using traditional way of velocity and acceleration.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPGPawnMovementComponent: Rotation")
	uint8 bOnlyMoveForward : 1;

protected:
	// The maximum falling speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"), Category = "RPGPawnMovementComponent")
	float MaxFallSpeed;

	// The maximum ground speed when walking. Also determines maximum lateral speed when falling.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"), Category = "RPGPawnMovementComponent")
	float MaxWalkSpeed;

	// The maximum speed when flying
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"), Category = "RPGPawnMovementComponent")
	float MaxFlySpeed;

	// The maximum speed when swimming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"), Category = "RPGPawnMovementComponent")
	float MaxSwimSpeed;

	// Max Acceleration (rate of change of velocity)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"), Category = "RPGPawnMovementComponent")
	float MaxAcceleration;

private:
	// Max angle in degrees of a walkable surface. Any greater than this and it is too steep to be walkable.
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "90.0", UIMin = "0.0", UIMax = "90.0"), Category = "RPGPawnMovementComponent")
	float WalkableFloorAngle;

	// Minimum Z value for floor normal. If less, not a walkable surface. Computed from WalkableFloorAngle.
	UPROPERTY(VisibleAnywhere, Category = "LS Pawn Movement")
	float WalkableFloorZ;

protected:
	// Current acceleration vector (with magnitude).
	// This is calculated each update based on the input vector and the constraints of MaxAcceleration and the current movement mode.
	UPROPERTY()
	FVector Acceleration;

public: // Path Following
	// If true, acceleration is applied when path following to reach the target velocity.
	// If false, path following velocity is set directly, disregarding acceleration. Also ignore bOnlyMoveForward.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "RPGPawnMovementComponent: Path Following")
	// Should use acceleration for path following?
	uint8 bRequestedMoveUseAcceleration : 1;

protected:

	UPROPERTY(Transient)
	// Was velocity requested by path following?
	uint8 bHasRequestedVelocity : 1;

	UPROPERTY(Transient)
	// Velocity requested by path following.
	FVector RequestedVelocity;

private:

	// The trace distance from center of the pawn downward
	float FallTraceDistance = 0;

	// Current falling speed
	float FallSpeed = 0;

	// Result from single vertical line trace from the location of the pawn
	// Used for walking pawns to move along the floor, or enter falling state.
	FHitResult CurrentFloorTraceResult;
	
public:
	// Cache Current Locomotion State to update GetMaxSpeed
	FRPGLocomotionState CurrentLocomotionState;
};
