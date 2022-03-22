// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RPGGameplayTags.h"
#include "RPGAnimationTypes.generated.h"

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
	UPROPERTY(BlueprintReadOnly, Category = "LS|Anim Graph - Poses")
	FGameplayTag BaseLayerPose;

	UPROPERTY(BlueprintReadOnly, Category = "LS|Anim Graph - Poses")
	FGameplayTag OverlayLayerPose;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGAnimPosesChangedNDynamicDelegate, const FRPGAnimPoses&, newAnimPoses);
DECLARE_MULTICAST_DELEGATE_OneParam(FRPGAnimPosesChangedNativeDelegate, const FRPGAnimPoses&);