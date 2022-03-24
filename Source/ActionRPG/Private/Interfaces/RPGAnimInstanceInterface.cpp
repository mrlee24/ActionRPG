// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/RPGAnimInstanceInterface.h"


// Add default functionality here for any IRPGAnimInstanceInterface functions that are not pure virtual.
FVector IRPGAnimInstanceInterface::GetSocketLocation(const FName& socketName) const
{
	return GetSkeletalMeshComponent()->GetSocketLocation(socketName);
}
