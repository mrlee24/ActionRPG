// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animations/RPGAnimationTypes.h"
#include "Components/ActorComponent.h"
#include "RPGAnimMasterComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(RPGMasterAnimComponentLog, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPG_API URPGAnimMasterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URPGAnimMasterComponent(const FObjectInitializer& objectInitializer);

protected: // UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	protected:
	void NotifyAnimPoses(const FRPGAnimPoses& newValue);

	virtual void HandleRotationSpeedChanged();
	virtual void HandleRotationMethodChanged();

	UFUNCTION()
	void OnRep_AnimPoses(const FRPGAnimPoses& newValue);

	UFUNCTION()
	void OnRep_RotationMethod();

	UFUNCTION()
	void OnRep_RotationSpeed();

	UFUNCTION(Server, Reliable)
	void Server_SetAnimPoses(const FRPGAnimPoses& newAnimPoses);

	UFUNCTION(Server, Reliable)
	void Server_SetRotation(
			const ERPGRotationMethod newRotationMethod,
			const float newRotationSpeed,
			const float newTurnStartAngle,
			const float newTurnStopTolerance);

	UFUNCTION(Server, Reliable)
	void Server_SetupAimOffset(const ERPGAimOffsets newAimOffsetType, const ERPGAimOffsetClamp newAimBehavior);

	UFUNCTION(Server, Reliable)
	void Server_SetAimOffset(const FRotator& newAimOffset);

	UFUNCTION(Server, Reliable)
	void Server_SetLookAt(const FVector& newLookAt);

	UFUNCTION(NetMulticast, Unreliable)
	virtual void Client_SetLooktAt(const FVector& newLookAt);

	UFUNCTION(NetMulticast, Unreliable)
	virtual void Client_SetAimOffset(const FRotator& newAimOffset);

#if WITH_EDITOR
	/** Setup and register Gameplay Debug Widget. Handles input binding for activation of the widget */
	virtual void SetupDebug();
#endif

	virtual void AimTick();

public:
	UFUNCTION(BlueprintCallable, Category = "RPG|Animation|Poses")
	void SetupAnimPoses(const FRPGAnimPoses& newAnimPoses);

	UFUNCTION(BlueprintCallable, Category = "RPG|Animation|Rotation")
	void SetupRotation(
			const ERPGRotationMethod newRotationMethod = ERPGRotationMethod::ERotateToVelocity,
			const float newRotationSpeed = 360.f,
			const float newTurnStartAngle = 90.f,
			const float newTurnStopTolerance = 5.f,
			const bool newbInterpRotation = true,
			const float newRotationInterpSpeed = 10.f);

	UFUNCTION(BlueprintCallable, Category = "RPG|Animation|Rotation")
	void SetupAimOffset(
			const ERPGAimOffsets newAimOffsetType = ERPGAimOffsets::ELook,
			const ERPGAimOffsetClamp newAimOffsetBehavior = ERPGAimOffsetClamp::ENearest,
			const float newAimClamp = 90.f,
			const bool bNewCameraBased = true,
			const FName newAimSocketName = "hand_r",
			const FName newLookAtSocketName = "head");

	UFUNCTION(BlueprintCallable, Category = "RPG|Animation|Rotation")
	bool SetActorRotation(const FRotator& targetRotation, const bool bUseInterpRotation, const float interpSpeed) const;

private:
	void HandleDesiredAtAngle();
	void HandleDesiredRotation();
	void HandleRotateToVelocity();
 
public:
	FRPGAnimPosesChangedNativeDelegate& GetAnimPosesNativeDelegate();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = "OnRep_AnimPoses", Category = "RPG|Animation|Setup")
	FRPGAnimPoses AnimPoses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG|Animation|Setup")
	ERPGRotationMethod RotationMethod = ERPGRotationMethod::ERotateToVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "RPG|Animation|Setup")
	float RotationSpeed = 360.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "RPG|Animation|Setup")
	float TurnStartAngle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "RPG|Animation|Setup")
	float TurnStopTolerance = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "RPG|Animation|Setup")
	uint8 bInterpRotation : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "RPG|Animation|Setup")
	float RotationInterpSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "RPG|Animation|Setup")
	ERPGAimOffsets AimOffsetType = ERPGAimOffsets::ELook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "RPG|Animation|Setup")
	ERPGAimOffsetClamp AimOffsetBehavior = ERPGAimOffsetClamp::ENearest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG|Animation|Setup")
	float AimClamp = 135.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG|Animation|Setup")
	uint8 bCameraBased : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG|Animation|Setup")
	FName AimSocketName = "hand_r";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG|Animation|Setup")
	FName LookAtSocketName = "Spine_03";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG|Animation|Runtime")
	FRotator AimOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG|Animation|Runtime")
	FVector LookAtLocation;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG|Animation|Debug")
	uint8 bDebug : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG|Animation|Debug")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_Visibility;

	UPROPERTY(EditDefaultsOnly, Category = "AGR|Debug")
	// Color of the line drawn to display when the starting point is LookAtSocket.
	FColor LookAtLineColor;

	UPROPERTY(EditDefaultsOnly, Category = "AGR|Debug")
	// Color of the line drawn to display when the starting point is AimSocket.
	FColor AimLineColor;

	UPROPERTY(EditDefaultsOnly, Category = "AGR|Debug")
	bool bLinePersists;

	UPROPERTY(EditDefaultsOnly, Category = "AGR|Debug")
	float LineThickness;

	UPROPERTY(EditDefaultsOnly, Category = "AGR|Debug")
	float LineLifetime;
#endif

protected:
	UPROPERTY(Transient, DuplicateTransient)
	class APawn* OwnerPawn;

	UPROPERTY(BlueprintReadOnly, Category = "Pawn")
	class UMovementComponent* MovementComponent;

public:
	UPROPERTY(BlueprintAssignable, Category = "RPGMasterAnimComponent")
	FRPGAnimPosesChangedNDynamicDelegate OnAnimPosesChangedDynamicDelegate;

protected:
	FRPGAnimPosesChangedNativeDelegate OnAnimPosesChangedNativeDelegate;
};