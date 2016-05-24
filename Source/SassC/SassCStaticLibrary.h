// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "SassCStaticLibrary.generated.h"

/**
 * Credit to Rama for Trace functions
 * See https://wiki.unrealengine.com/Trace_Functions
 */
UCLASS()
class SASSC_API USassCStaticLibrary : public UObject
{
	GENERATED_BODY()
	
public:
	static FORCEINLINE bool Trace(
		UWorld* World,
		AActor* ActorToIgnore,
		const FVector& Start,
		const FVector& End,
		FHitResult& HitOut,
		FCollisionObjectQueryParams& ObjectQueryParams,
		bool ReturnPhysMat = false
		)
	{
		if (!World)
		{
			return false;
		}

		FCollisionQueryParams TraceParams(FName(TEXT("Line Trace by Object")), true, ActorToIgnore);
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = ReturnPhysMat;
		TraceParams.AddIgnoredActor(ActorToIgnore);
		HitOut = FHitResult(ForceInit);
		World->LineTraceSingleByObjectType(HitOut, Start, End, ObjectQueryParams, TraceParams);
		return (HitOut.GetActor() != NULL);
	}

	static FORCEINLINE bool Trace(
		UWorld* World,
		TArray<AActor*>& ActorsToIgnore,
		const FVector& Start,
		const FVector& End,
		FHitResult& HitOut,
		FCollisionObjectQueryParams& ObjectQueryParams,
		bool ReturnPhysMat = false
		)
	{
		if (!World)
		{
			return false;
		}

		FCollisionQueryParams TraceParams(FName(TEXT("Line Trace by Object")), true, ActorsToIgnore[0]);
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = ReturnPhysMat;
		TraceParams.AddIgnoredActors(ActorsToIgnore);
		HitOut = FHitResult(ForceInit);
		World->LineTraceSingleByObjectType(HitOut, Start, End, ObjectQueryParams, TraceParams);
		return (HitOut.GetActor() != NULL);
	}

	static FORCEINLINE bool Trace(
		UWorld* World,
		AActor* ActorToIgnore,
		const FVector& Start,
		const FVector& End,
		FHitResult& HitOut,
		ECollisionChannel CollisionChannel = ECC_Pawn,
		bool ReturnPhysMat = false
		) 
	{
		if (!World)
		{
			return false;
		}

		FCollisionQueryParams TraceParams(FName(TEXT("Line Trace by Channel")), true, ActorToIgnore);
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = ReturnPhysMat;
		TraceParams.AddIgnoredActor(ActorToIgnore);
		HitOut = FHitResult(ForceInit);
		World->LineTraceSingleByChannel(HitOut, Start, End, CollisionChannel, TraceParams);
		return (HitOut.GetActor() != NULL);
	}
	
	static FORCEINLINE bool Trace(
		UWorld* World,
		TArray<AActor*>& ActorsToIgnore,
		const FVector& Start,
		const FVector& End,
		FHitResult& HitOut,
		ECollisionChannel CollisionChannel = ECC_Pawn,
		bool ReturnPhysMat = false
		) 
	{
		if (!World)
		{
			return false;
		}

		FCollisionQueryParams TraceParams(FName(TEXT("Line Trace by Channel")), true, ActorsToIgnore[0]);
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = ReturnPhysMat;
		TraceParams.AddIgnoredActors(ActorsToIgnore);
		HitOut = FHitResult(ForceInit);
		World->LineTraceSingleByChannel(HitOut, Start, End, CollisionChannel, TraceParams);
		return (HitOut.GetActor() != NULL);
	}
};
