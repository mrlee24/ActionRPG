// Fill out your copyright notice in the Description page of Project Settings.


#include "Collision/RPGCollisionManagerComponent.h"

#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(RPGCollisionsManagerComponentLog);
DECLARE_STATS_GROUP(TEXT("RPGCollisionManagerComponent"), STATGROUP_RPGCollisionManager, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("RPGCollisionManagerComponent - UpdateCollisions"), STAT_RPGCollisionManager_UpdateCollisions,
STATGROUP_RPGCollisionManager);

// Sets default values for this component's properties
URPGCollisionManagerComponent::URPGCollisionManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void URPGCollisionManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	SetComponentTickEnabled(false);
	SetCollisionsEnabled(false);

	if (CollisionInfo.DamageTraces.Num())
	{
		UpdateCollisionSettings(CollisionInfo);
	}

	TraceDelegate.BindUObject(this, &ThisClass::OnTraceCompleted);
}

void URPGCollisionManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasBegunPlay())
	{
		SetCollisionsEnabled(false);
	}

	TraceDelegate.Unbind();
	Super::EndPlay(EndPlayReason);
}

void URPGCollisionManagerComponent::SetCollisionsEnabled(bool bStarted)
{
	bHasCollisionsEnabled = bStarted;

	// Allow Collision Master component to tick us
	/*ALSGameState* myGameState = Cast<ALSGameState>(UGameplayStatics::GetGameState(this));
	if (myGameState)
	{
		ULSCollisionMasterComponent* collisionMasterComponent = myGameState->GetMasterCollisionComponent();
		if (collisionMasterComponent)
		{
			return inStarted
				? collisionMasterComponent->AddCollisionManagerComponent(this)
				: collisionMasterComponent->RemoveCollisionManagerComponent(this);
		}
	}*/
}

void URPGCollisionManagerComponent::UpdateCollisions()
{
	SCOPE_CYCLE_COUNTER(STAT_RPGCollisionManager_UpdateCollisions);

	UWorld* world = GetWorld();
	if (world && DamageMesh)
	{
		if (TracesPendingDelete.IsValidIndex(0))
		{
			AlreadyHitActors.Empty();
			for (const FName& tracesToDelete : TracesPendingDelete)
			{
				if (ActivatedTraces.Contains(tracesToDelete))
				{
					ActivatedTraces.Remove(tracesToDelete);
				}
			}
		}

		if (ActivatedTraces.Num() == 0)
		{
			SetCollisionsEnabled(false);
			return;
		}

		if (CollisionChannels.IsValidIndex(0))
		{
			for (const TPair<FName, FRPGTraceInfo>& currentTrace : ActivatedTraces)
			{
				if (currentTrace.Key == NAME_None)
				{
					continue;
				}

				if (DamageMesh->DoesSocketExist(currentTrace.Value.StartSocket) &&
					DamageMesh->DoesSocketExist(currentTrace.Value.EndSocket))
				{
					//Reset data relate to async trace
					TraceHandleToCurrentTrace.Reset();
					HitActorsInThisFrame.Reset();

					const FVector curStartPos = DamageMesh->GetSocketLocation(currentTrace.Value.StartSocket);
					const FVector curEndPos = DamageMesh->GetSocketLocation(currentTrace.Value.EndSocket);
					bool useSubStep = false;
					int32 step = 1;
					int32 totalSteps = 1;
					FVector startPos, endPos, previousStartPos, previousEndPos;

					// compute total steps and store previous positions if needed
					if (currentTrace.Value.MaxSubSteps > 0 && currentTrace.Value.SubStepMaxDistance > KINDA_SMALL_NUMBER)
					{
						if (PreviousStartPositions.Contains(currentTrace.Key) && PreviousEndPositions.Contains(currentTrace.Key))
						{
							useSubStep = true;
							previousStartPos = PreviousStartPositions.FindRef(currentTrace.Key);
							previousEndPos = PreviousEndPositions.FindRef(currentTrace.Key);
							const float endPosDist = FVector::Distance(curEndPos, previousEndPos);
							totalSteps = 1 + FMath::Min(currentTrace.Value.MaxSubSteps, FMath::FloorToInt(endPosDist / currentTrace.Value.SubStepMaxDistance));
						}
						PreviousStartPositions.Emplace(currentTrace.Key, curStartPos);
						PreviousEndPositions.Emplace(currentTrace.Key, curEndPos);
					}

					// Construct CollisionQueryParams
					FCollisionQueryParams collisionQueryParams;

					if (IgnoredActors.Num() > 0)
					{
						collisionQueryParams.AddIgnoredActors(IgnoredActors);
					}

					if (!bAllowMultipleHitsPerSwing)
					{
						FRPGHitActorsInfo* hitActorsInfo = AlreadyHitActors.Find(currentTrace.Key);
						if (hitActorsInfo && hitActorsInfo->AlreadyHitActors.Num() > 0)
						{
							for (auto& hitActor : hitActorsInfo->AlreadyHitActors)
							{
								collisionQueryParams.AddIgnoredActor(hitActor.Get());
							}
						}
					}

					collisionQueryParams.bReturnPhysicalMaterial = true;
					collisionQueryParams.bTraceComplex = true;

					// Construct CollisionObjectQueryParams
					FCollisionObjectQueryParams objectParams;
					for (ECollisionChannel channel : CollisionChannels)
					{
						objectParams.AddObjectTypesToQuery(channel);
					}

					if (objectParams.IsValid() == false)
					{
						UE_LOG(LogTemp, Warning, TEXT("Invalid Collision Channel"));
						return;
					}

					// Perform traces in steps
					while (step <= totalSteps)
					{
						startPos = useSubStep ? ComputeLocationsAtStep(previousStartPos, curStartPos, step, totalSteps) : curStartPos;
						endPos = useSubStep ? ComputeLocationsAtStep(previousEndPos, curEndPos, step, totalSteps) : curEndPos;

						FTraceHandle handle = world->AsyncSweepByObjectType(EAsyncTraceType::Single, startPos, endPos, FQuat::Identity,
							objectParams, FCollisionShape::MakeSphere(currentTrace.Value.Radius), collisionQueryParams,
							&TraceDelegate);
						TraceHandleToCurrentTrace.Emplace(handle, currentTrace.Key);

						if (bDrawDebug)
						{
							FHitResult _hit;
							UKismetSystemLibrary::SphereTraceSingle(this, startPos, endPos, currentTrace.Value.Radius,
								TraceTypeQuery1, false, IgnoredActors,
								EDrawDebugTrace::ForDuration, _hit, true, FColor::Green);
						}

						step++;
					}
				}
				else
				{
					UE_LOG(RPGCollisionsManagerComponentLog, Warning, TEXT("Invalid Socket Names!!, %s, %s, %s, %s"),
						*DamageMesh->GetName(), *currentTrace.Key.ToString(),
						*currentTrace.Value.StartSocket.ToString(), *currentTrace.Value.EndSocket.ToString());
				}
			}
		}
		else
		{
			SetCollisionsEnabled(false);
		}
	}
}

void URPGCollisionManagerComponent::HandleTimedSingleTraceFinished(const FName& traceEnded)
{
	if (GetOwner())
	{
		UWorld* world = GetWorld();
		if (world && AllTraceTimers.Contains(traceEnded))
		{
			FTimerHandle* handle = AllTraceTimers.Find(traceEnded);
			world->GetTimerManager().ClearTimer(*handle);
		}
	}
}

void URPGCollisionManagerComponent::HandleAllTimedTraceFinished()
{
	StopAllTraces();
	if (GetOwner())
	{
		UWorld* world = GetWorld();
		if (world && bAllTimedTraceStarted)
		{
			world->GetTimerManager().ClearTimer(AllTraceTimer);
			bAllTimedTraceStarted = false;
		}
	}
}

void URPGCollisionManagerComponent::SetupCollisionManager(UMeshComponent* inDamageMesh)
{
	DamageMesh = inDamageMesh;

	if (!DamageMesh)
	{
		UE_LOG(RPGCollisionsManagerComponentLog, Warning, TEXT("Invalid Damage mesh!!"));
	}
}

void URPGCollisionManagerComponent::AddActorToIgnore(AActor* ignoredActor)
{
	IgnoredActors.AddUnique(ignoredActor);
}

void URPGCollisionManagerComponent::AddCollisionChannel(const TEnumAsByte<ECollisionChannel>& inTraceChannel)
{
	CollisionChannels.AddUnique(inTraceChannel);
}

void URPGCollisionManagerComponent::SetCollisionChannels(const TArray<TEnumAsByte<ECollisionChannel>>& inTraceChannels)
{
	ClearCollisionChannels();

	for (const TEnumAsByte<ECollisionChannel>& traceChannel : inTraceChannels)
	{
		AddCollisionChannel(traceChannel);
	}
}

void URPGCollisionManagerComponent::ClearCollisionChannels()
{
	CollisionChannels.Reset();
}

void URPGCollisionManagerComponent::StartAllTraces()
{
	ActivatedTraces.Empty();
	TracesPendingDelete.Empty();

	for (const auto& damage : DamageTraces)
	{
		StartSingleTrace(damage.Key);
	}
}

void URPGCollisionManagerComponent::StopAllTraces()
{
	TracesPendingDelete.Empty();
	for (const auto& trace : ActivatedTraces)
	{
		StopSingleTrace(trace.Key);
	}
}

void URPGCollisionManagerComponent::StartSingleTrace(const FName& name)
{
	FRPGTraceInfo* traceInfo = DamageTraces.Find(name);
	if (traceInfo)
	{
		if (TracesPendingDelete.Contains(name))
		{
			TracesPendingDelete.Remove(name);
		}

		// Reset previous socket locations for this trace info
		if (PreviousStartPositions.Contains(name))
		{
			PreviousStartPositions.Remove(name);
		}

		if (PreviousEndPositions.Contains(name))
		{
			PreviousEndPositions.Remove(name);
		}

		ActivatedTraces.Add(name, *traceInfo);
		SetCollisionsEnabled(true);
	}
	else
	{
		UE_LOG(RPGCollisionsManagerComponentLog, Warning, TEXT("Invalid Trace Name!!"));
	}
}

void URPGCollisionManagerComponent::StopSingleTrace(const FName& name)
{
	if (ActivatedTraces.Contains(name))
	{
		TracesPendingDelete.AddUnique(name);

		FRPGHitActorsInfo* hitActorsInfo = AlreadyHitActors.Find(name);
		if (hitActorsInfo)
		{
			hitActorsInfo->AlreadyHitActors.Empty();
		}
	}
}

void URPGCollisionManagerComponent::StartTimedSingleTrace(const FName& traceName, float duration)
{
	UWorld* world = GetWorld();
	if (world)
	{
		StartSingleTrace(traceName);
		FTimerHandle timerHandle;
		FTimerDelegate timerDelegate = FTimerDelegate::CreateWeakLambda(this, [this](const FName& traceName)
			{
				HandleTimedSingleTraceFinished(traceName);
			}, traceName);
		AllTraceTimers.Add(traceName, timerHandle);
		world->GetTimerManager().SetTimer(timerHandle, MoveTemp(timerDelegate), duration, false);
	}
}

void URPGCollisionManagerComponent::StartAllTimedTraces(float duration)
{
	UWorld* world = GetWorld();
	if (world && !bAllTimedTraceStarted)
	{
		StartAllTraces();
		world->GetTimerManager().SetTimer(AllTraceTimer, this,
			&ThisClass::HandleAllTimedTraceFinished, duration,
			false);
		bAllTimedTraceStarted = true;
	}
}

void URPGCollisionManagerComponent::Server_StartAllTraces_Implementation()
{
	StartAllTraces();
}

bool URPGCollisionManagerComponent::Server_StartAllTraces_Validate()
{
	return true;
}

void URPGCollisionManagerComponent::Server_StopAllTraces_Implementation()
{
	StopAllTraces();
}

bool URPGCollisionManagerComponent::Server_StopAllTraces_Validate()
{
	return true;
}

void URPGCollisionManagerComponent::Server_StartSingleTrace_Implementation(const FName& name)
{
	StartSingleTrace(name);
}

bool URPGCollisionManagerComponent::Server_StartSingleTrace_Validate(const FName& name)
{
	return true;
}

void URPGCollisionManagerComponent::Server_StopSingleTrace_Implementation(const FName& name)
{
	StopSingleTrace(name);
}

bool URPGCollisionManagerComponent::Server_StopSingleTrace_Validate(const FName& name)
{
	return true;
}

void URPGCollisionManagerComponent::Server_StartTimedSingleTrace_Implementation(const FName& traceName, float duration)
{
	StartTimedSingleTrace(traceName, duration);
}

bool URPGCollisionManagerComponent::Server_StartTimedSingleTrace_Validate(const FName& traceName, float duration)
{
	return true;
}

void URPGCollisionManagerComponent::Server_StartAllTimedTraces_Implementation(float duration)
{
	StartAllTimedTraces(duration);
}

bool URPGCollisionManagerComponent::Server_StartAllTimedTraces_Validate(float duration)
{
	return true;
}

const TMap<FName, FRPGTraceInfo>& URPGCollisionManagerComponent::GetDamageTraces() const
{
	return DamageTraces;
}

FRPGCollisionDetectedNativeDelegate& URPGCollisionManagerComponent::GetCollisionDetectedNativeDelegate()
{
	return CollisionDetectedNativeDelegate;
}

void URPGCollisionManagerComponent::SetTraceConfig(const FName& traceName, const FRPGTraceInfo& traceInfo)
{
	DamageTraces.Emplace(traceName, traceInfo);
}

bool URPGCollisionManagerComponent::TryGetTraceConfig(const FName& traceName, FRPGTraceInfo& outTraceInfo) const
{
	if (DamageTraces.Contains(traceName))
	{
		outTraceInfo = *DamageTraces.Find(traceName);
		return true;
	}
	return false;
}

void URPGCollisionManagerComponent::UpdateCollisionSettings(const FRPGCollisionInfo& collisionData)
{
	for (const auto& trace : collisionData.DamageTraces)
	{
		SetTraceConfig(trace.Key, trace.Value);
	}

	SetCollisionChannels(collisionData.CollisionChannels);
	for (AActor* actor : collisionData.IgnoredActors)
	{
		AddActorToIgnore(actor);
	}

	bAllowMultipleHitsPerSwing = collisionData.bAllowMultipleHitsPerSwing;
	bDrawDebug = collisionData.bCollisionDrawDebug;
}

UMeshComponent* URPGCollisionManagerComponent::GetMeshComponent() const
{
	return DamageMesh;
}

FVector URPGCollisionManagerComponent::ComputeLocationsAtStep(const FVector& previous, const FVector& current,
	const int32 curStep, const int32 totalSteps)
{
	return previous + (current - previous) / totalSteps * curStep;
}

void URPGCollisionManagerComponent::OnTraceCompleted(const FTraceHandle& handle, FTraceDatum& data)
{
	if (TraceHandleToCurrentTrace.Contains(handle))
	{
		FName currentTraceName = TraceHandleToCurrentTrace.FindAndRemoveChecked(handle);
		if (data.OutHits.Num() > 0)
		{
			FHitResult hitResult = data.OutHits[0];
			AActor* hitActor = hitResult.GetActor();
			FRPGHitActorsInfo* hitActorsInThisFrameCurrentTrace = HitActorsInThisFrame.Find(currentTraceName);

			// If actor has already been hit in the same frame on the same FTowersTraceInfo, skip it. Otherwise add it to already hit set.
			if (!hitActor || (hitActorsInThisFrameCurrentTrace && hitActorsInThisFrameCurrentTrace->AlreadyHitActors.Contains(hitActor)))
			{
				return;
			}

			AddHitActorToSet(hitActor, currentTraceName, HitActorsInThisFrame);

			OnCollisionDetected.Broadcast(currentTraceName, hitResult);
			CollisionDetectedNativeDelegate.Broadcast(currentTraceName, hitResult);

			if (!bAllowMultipleHitsPerSwing)
			{
				AddHitActorToSet(hitActor, currentTraceName, AlreadyHitActors);
			}
		}
	}
}

void URPGCollisionManagerComponent::AddHitActorToSet(AActor* actor, const FName& traceName,
	TMap<FName, FRPGHitActorsInfo>& outHitActorInfo)
{
	FRPGHitActorsInfo* hitActorsInfo = outHitActorInfo.Find(traceName);
    if (hitActorsInfo)
    {
    	hitActorsInfo->AlreadyHitActors.Add(actor);
    }
    else
    {
    	FRPGHitActorsInfo newHitActorsInfo;
    	newHitActorsInfo.AlreadyHitActors.Add(actor);
    	outHitActorInfo.Add(traceName, MoveTemp(newHitActorsInfo));
    }
}
