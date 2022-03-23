// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Components/RPGAnimMasterComponent.h"

#include "Interfaces/RPGMovableInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(RPGMasterAnimComponentLog);

URPGAnimMasterComponent::URPGAnimMasterComponent(const FObjectInitializer& objectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

#if WITH_EDITOR
	bDebug = false;
	bLinePersists = false;
	LookAtLineColor = FColor::Blue;
	AimLineColor = FColor::Red;
	LineThickness = 1.0f;
	LineLifetime = 0.0f;
#endif

	RotationMethod = ERPGRotationMethod::ERotateToVelocity;
	AimOffsetType = ERPGAimOffsets::ELook;
	AimOffsetBehavior = ERPGAimOffsetClamp::ELeft;
	bCameraBased = true;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void URPGAnimMasterComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetOwner())
	{
		UE_LOG(RPGMasterAnimComponentLog, Warning, TEXT("This component has not been attached to any actors"));
		return;
	}

	OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		UE_LOG(RPGMasterAnimComponentLog, Warning, TEXT("OwnerPawn is invalid"));
		return;
	}
	
	SetupAnimPoses(AnimPoses);
	SetupRotation(RotationMethod, RotationSpeed, TurnStartAngle, TurnStopTolerance);
	SetupAimOffset(AimOffsetType, AimOffsetBehavior, AimClamp, bCameraBased, AimSocketName, LookAtSocketName);

#if WITH_EDITOR
	SetupDebug();
#endif
}

void URPGAnimMasterComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, AnimPoses);
	DOREPLIFETIME(ThisClass, RotationMethod);
	DOREPLIFETIME(ThisClass, RotationSpeed);
	DOREPLIFETIME(ThisClass, TurnStartAngle);
	DOREPLIFETIME(ThisClass, TurnStopTolerance);
	DOREPLIFETIME(ThisClass, bInterpRotation);
	DOREPLIFETIME(ThisClass, RotationInterpSpeed);
	DOREPLIFETIME(ThisClass, AimOffsetType);
	DOREPLIFETIME(ThisClass, AimOffsetBehavior);
}

void URPGAnimMasterComponent::NotifyAnimPoses(const FRPGAnimPoses& newValue)
{
	OnAnimPosesChangedDynamicDelegate.Broadcast(newValue);
	OnAnimPosesChangedNativeDelegate.Broadcast(newValue);
}

void URPGAnimMasterComponent::HandleRotationSpeedChanged()
{
	// Implement this in sub class
}

void URPGAnimMasterComponent::HandleRotationMethodChanged()
{
	switch (RotationMethod)
	{
	case ERPGRotationMethod::EDesiredAtAngle:
		HandleDesiredAtAngle();
		break;
	case ERPGRotationMethod::EDesiredRotation:
		HandleDesiredRotation();
		break;
	case ERPGRotationMethod::ERotateToVelocity:
		HandleRotateToVelocity();
		break;
	default:
		checkNoEntry();
	}
}

void URPGAnimMasterComponent::OnRep_AnimPoses(const FRPGAnimPoses& newValue)
{
	NotifyAnimPoses(newValue);
}

void URPGAnimMasterComponent::OnRep_RotationMethod()
{
	HandleRotationMethodChanged();
}

void URPGAnimMasterComponent::OnRep_RotationSpeed()
{
	HandleRotationSpeedChanged();
}

void URPGAnimMasterComponent::Server_SetAnimPoses_Implementation(const FRPGAnimPoses& newAnimPoses)
{
	AnimPoses = newAnimPoses;

	if (!OwnerPawn->IsLocallyControlled())
	{
		OnRep_AnimPoses(AnimPoses);
	}
}

bool URPGAnimMasterComponent::Server_SetAimOffset_Validate(const FRotator& newAimOffset)
{
	return true;
}

void URPGAnimMasterComponent::Server_SetRotation_Implementation(const ERPGRotationMethod newRotationMethod,
	const float newRotationSpeed, const float newTurnStartAngle, const float newTurnStopTolerance)
{
	RotationMethod = newRotationMethod;
	HandleRotationMethodChanged();
	RotationSpeed = newRotationSpeed;
	HandleRotationSpeedChanged();
	TurnStartAngle = newTurnStartAngle;
	TurnStopTolerance = newTurnStopTolerance;
}

void URPGAnimMasterComponent::Server_SetupAimOffset_Implementation(const ERPGAimOffsets newAimOffsetType,
	const ERPGAimOffsetClamp newAimBehavior)
{
	AimOffsetType = newAimOffsetType;
	AimOffsetBehavior = newAimBehavior;
}

bool URPGAnimMasterComponent::Server_SetupAimOffset_Validate(const ERPGAimOffsets newAimOffsetType,
	const ERPGAimOffsetClamp newAimBehavior)
{
	return true;
}

void URPGAnimMasterComponent::Server_SetAimOffset_Implementation(const FRotator& newAimOffset)
{
	NetMulticast_SetAimOffset(newAimOffset);
}

void URPGAnimMasterComponent::Server_SetLookAt_Implementation(const FVector& newLookAt)
{
	NetMulticast_SetLooktAt(newLookAt);
}

bool URPGAnimMasterComponent::Server_SetLookAt_Validate(const FVector& newLookAt)
{
	return true;
}

void URPGAnimMasterComponent::NetMulticast_SetLooktAt_Implementation(const FVector& newLookAt)
{
	if (OwnerPawn && !OwnerPawn->IsLocallyControlled())
	{
		LookAtLocation = newLookAt;
	}
}

bool URPGAnimMasterComponent::NetMulticast_SetLooktAt_Validate(const FVector& newLookAt)
{
	return true;
}

void URPGAnimMasterComponent::NetMulticast_SetAimOffset_Implementation(const FRotator& newAimOffset)
{
	if (OwnerPawn && !OwnerPawn->IsLocallyControlled())
	{
		AimOffset = newAimOffset;
	}
}

bool URPGAnimMasterComponent::NetMulticast_SetAimOffset_Validate(const FRotator& newAimOffset)
{
	return true;
}

void URPGAnimMasterComponent::SetupDebug()
{
	// Implement this in sub class
}

void URPGAnimMasterComponent::AimTick()
{
	// Implement this in subclass
}

void URPGAnimMasterComponent::SetupAnimPoses(const FRPGAnimPoses& newAnimPoses)
{
	AnimPoses.BaseLayerPose = newAnimPoses.BaseLayerPose;
	AnimPoses.OverlayLayerPose = newAnimPoses.OverlayLayerPose;
	NotifyAnimPoses(AnimPoses);
	Server_SetAnimPoses(AnimPoses);
}

void URPGAnimMasterComponent::SetupRotation(const ERPGRotationMethod newRotationMethod, const float newRotationSpeed,
	const float newTurnStartAngle, const float newTurnStopTolerance, const bool newbInterpRotation,
	const float newRotationInterpSpeed)
{
	RotationMethod = newRotationMethod;
	bInterpRotation = newbInterpRotation;
	RotationInterpSpeed = newRotationInterpSpeed;
	HandleRotationMethodChanged();
	RotationSpeed = newRotationSpeed;
	HandleRotationSpeedChanged();
	TurnStartAngle = newTurnStartAngle;
	TurnStopTolerance = newTurnStopTolerance;
}

void URPGAnimMasterComponent::SetupAimOffset(const ERPGAimOffsets newAimOffsetType,
	const ERPGAimOffsetClamp newAimOffsetBehavior, const float newAimClamp, const bool bNewCameraBased,
	const FName newAimSocketName, const FName newLookAtSocketName)
{
	AimOffsetType = newAimOffsetType;
	AimOffsetBehavior = newAimOffsetBehavior;
	AimClamp = newAimClamp;
	bCameraBased = bNewCameraBased;
	AimSocketName = newAimSocketName;
	LookAtSocketName = newLookAtSocketName;

	Server_SetupAimOffset(newAimOffsetType, newAimOffsetBehavior);
}

bool URPGAnimMasterComponent::SetActorRotation(const FRotator& targetRotation, const bool bUseInterpRotation,
	const float interpSpeed) const
{
	if (bUseInterpRotation && (interpSpeed != 0.f))
	{
		const FRotator controlRotation = FMath::RInterpTo(OwnerPawn->GetActorRotation(), targetRotation, GetWorld()->GetDeltaSeconds(), interpSpeed);
		return OwnerPawn->SetActorRotation(controlRotation);
	}

	return OwnerPawn->SetActorRotation(targetRotation);
}

void URPGAnimMasterComponent::HandleDesiredAtAngle()
{
	if (OwnerPawn)
	{
		const float deltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(AimOffset, OwnerPawn->GetActorRotation()).Yaw;
		const float speed = OwnerPawn->GetVelocity().SizeSquared();
		if (FMath::Abs(deltaYaw) > TurnStartAngle || speed > 50.f)
		{
			HandleDesiredRotation();
		}
		else 
		{
			const float clampValue = FMath::Clamp(TurnStopTolerance, 1.f, 90.f);
			const float minClamp = clampValue * -1.f;
			const float maxClamp = clampValue;
			const bool bInRange = UKismetMathLibrary::InRange_FloatFloat(deltaYaw, minClamp, maxClamp);
			if (bInRange)
			{
				HandleRotateToVelocity();
			}
		}
	}
}

void URPGAnimMasterComponent::HandleDesiredRotation()
{
	if (OwnerPawn)
	{
		SetActorRotation(
			FRotator(0.f, OwnerPawn->GetControlRotation().Yaw, 0.f),
				bInterpRotation, RotationInterpSpeed);
	}
}

void URPGAnimMasterComponent::HandleRotateToVelocity()
{
	if (OwnerPawn)
	{
		const FRotator& velocityRotation = OwnerPawn->GetVelocity().ToOrientationRotator();

		// We only set the rotation if we have enough horizontal movement
		if (OwnerPawn->GetVelocity().SizeSquared() > .1f)
		{
			SetActorRotation(FRotator(0.f, velocityRotation.Yaw, 0.f),
				bInterpRotation, RotationInterpSpeed);
		}
	}
}

FRPGAnimPosesChangedNativeDelegate& URPGAnimMasterComponent::GetAnimPosesNativeDelegate()
{
	return OnAnimPosesChangedNativeDelegate;
}



