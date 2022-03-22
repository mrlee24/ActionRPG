#pragma once

#include "CoreMinimal.h"

/** Specifies why an actor is being deleted/removed from a level */
UENUM(BlueprintType)
namespace ERPGEndPlayReason
{
	enum Type
	{
		Undefined,
		/** When the Actor or Component is explicitly destroyed. */
		Destroyed,
		/** When the Actor is converted into a foliage. */
		ConvertedToFoliage,
		/** When the world is being unloaded for a level transition. */
		LevelTransition,
		/** When the world is being unloaded because PIE is ending. */
		EndPlayInEditor,
		/** When the level it is a member of is streamed out. */
		RemovedFromWorld,
		/** When the application is being exited. */
		Quit,
		/** When the Actor is de-spawned by the culling manager. */
		Culling,
	};
}