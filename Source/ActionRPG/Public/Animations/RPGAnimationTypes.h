// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RPGGameplayTags.h"
#include "RPGAnimationTypes.generated.h"

//////////////////////////////
///// ANIMATION STATES ///////
//////////////////////////////

static const FName NAME_BasePose_Crouching(TEXT("BasePose_Crouching"));
static const FName NAME_BasePose_Standing(TEXT("BasePose_Standing"));
static const FName NAME_Enable_FootIK_R(TEXT("Enable_FootIK_R"));
static const FName NAME_Enable_FootIK_L(TEXT("Enable_FootIK_L"));
static const FName NAME_Enable_HandIK_L(TEXT("Enable_HandIK_L"));
static const FName NAME_Enable_HandIK_R(TEXT("Enable_HandIK_R"));
static const FName NAME_Enable_Transition(TEXT("Enable_Transition"));
static const FName NAME_FootLock_L(TEXT("FootLock_L"));
static const FName NAME_FootLock_R(TEXT("FootLock_R"));
static const FName NAME_Grounded___Slot(TEXT("Grounded Slot"));
static const FName NAME_Layering_Arm_L(TEXT("Layering_Arm_L"));
static const FName NAME_Layering_Arm_L_Add(TEXT("Layering_Arm_L_Add"));
static const FName NAME_Layering_Arm_L_LS(TEXT("Layering_Arm_L_LS"));
static const FName NAME_Layering_Arm_R(TEXT("Layering_Arm_R"));
static const FName NAME_Layering_Arm_R_Add(TEXT("Layering_Arm_R_Add"));
static const FName NAME_Layering_Arm_R_LS(TEXT("Layering_Arm_R_LS"));
static const FName NAME_Layering_Hand_L(TEXT("Layering_Hand_L"));
static const FName NAME_Layering_Hand_R(TEXT("Layering_Hand_R"));
static const FName NAME_Layering_Head_Add(TEXT("Layering_Head_Add"));
static const FName NAME_Layering_Spine_Add(TEXT("Layering_Spine_Add"));
static const FName NAME_Mask_AimOffset(TEXT("Mask_AimOffset"));
static const FName NAME_Mask_LandPrediction(TEXT("Mask_LandPrediction"));
static const FName NAME__ALSCharacterAnimInstance__RotationAmount(TEXT("RotationAmount"));
static const FName NAME_VB___foot_target_l(TEXT("VB foot_target_l"));
static const FName NAME_VB___foot_target_r(TEXT("VB foot_target_r"));
static const FName NAME_W_Gait(TEXT("W_Gait"));
static const FName NAME__MannequinAnimInstance__root(TEXT("root"));

UENUM(BlueprintType)
enum class ERPGAimOffsetClamp : uint8
{
	ENearest = 0 UMETA(DisplayName = "Nearest"),
	ELeft = 1	UMETA(DisplayName = "Left"),
	ERight = 2	UMETA(DisplayName = "Right")
};

UENUM(BlueprintType)
enum class ERPGAimOffsets :uint8
{
	EAim  = 0 	UMETA(DisplayName = "Aim"),
	ELook = 1	UMETA(DisplayName = "Look")
};

UENUM(BlueprintType)
enum class ERPGRotationMethod : uint8
{
	ERotateToVelocity = 0	UMETA(DisplayName = "Rotate To Velocity"),
	EDesiredRotation = 1	UMETA(DisplayName = "Desired Rotation"),
	EDesiredAtAngle = 2	    UMETA(DisplayName = "Desired At Angle")
};

USTRUCT(BlueprintType)
struct FRPGAnimPoses
{
	GENERATED_BODY()

public:
	FRPGAnimPoses()
		: BaseLayerPose(FGameplayTag::RequestGameplayTag(GTag_Animation_Poses_BaseLayer_Default))
		, OverlayLayerPose(FGameplayTag::RequestGameplayTag(GTag_Animation_Poses_OverlayLayer_Default))
	{}

	FRPGAnimPoses(const FGameplayTag& newBaseLayerPose, const FGameplayTag& newOverlayLayerPose)
		: BaseLayerPose(newBaseLayerPose)
		, OverlayLayerPose(newOverlayLayerPose)
	{}

public:
	UPROPERTY(BlueprintReadOnly, Category = "RPGAnimPoses")
	FGameplayTag BaseLayerPose;

	UPROPERTY(BlueprintReadOnly, Category = "RPGAnimPoses")
	FGameplayTag OverlayLayerPose;
};

USTRUCT(BlueprintType)
struct FRPGAnimGraphLayerBlending
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	int32 OverlayOverrideState = 0;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float EnableAimOffset = 1.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float BasePoseStanding = 1.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float BasePoseCrouching = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Arm_L = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Arm_L_Add = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Arm_L_LS = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Arm_L_MS = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Arm_R = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Arm_R_Add = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Arm_R_LS = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Arm_R_MS = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Hand_L = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Hand_R = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Legs = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Legs_Add = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Pelvis = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Pelvis_Add = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Spine = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Spine_Add = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Head = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float Head_Add = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float EnableHandIK_L = 1.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RPGAnimGraphLayerBlending")
	float EnableHandIK_R = 1.0f;
};

//////////////////////////////
///// LOCOMOTION STATES //////
//////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGAnimPosesChangedNDynamicDelegate, const FRPGAnimPoses&, newAnimPoses);
DECLARE_MULTICAST_DELEGATE_OneParam(FRPGAnimPosesChangedNativeDelegate, const FRPGAnimPoses&);

USTRUCT(BlueprintType)
struct FRPGLocomotionState
{
	GENERATED_BODY()

	FRPGLocomotionState() {};

	FRPGLocomotionState(FGameplayTag state, float maxSpeed)
	{
		State = state;
		MaxSpeed = maxSpeed;
	};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF")
	FGameplayTag State;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF")
	float MaxSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ACF")
	FName CameraMovement;

	FORCEINLINE	bool operator < (const FRPGLocomotionState& other) const
	{
		return MaxSpeed < other.MaxSpeed;
	}

	FORCEINLINE	bool operator == (const FRPGLocomotionState& other) const
	{
		return State == other.State;
	}

	FORCEINLINE	bool operator != (const FGameplayTag& other) const
	{
		return State != other;
	}

	FORCEINLINE	bool operator == (const FGameplayTag& other) const
	{
		return State == other;
	}

	FORCEINLINE	bool operator != (const FRPGLocomotionState& other) const
	{
		return State != other.State;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGLocomotionStateChanged, const FRPGLocomotionState&, locomotionState);
DECLARE_MULTICAST_DELEGATE_OneParam(FRPGLocomotionStateNativeChanged, const FRPGLocomotionState&);