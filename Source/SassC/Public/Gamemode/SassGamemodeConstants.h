// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SASSC_API SassGamemodeConstants
{
public:
	SassGamemodeConstants();
	~SassGamemodeConstants();

	static float GetGraphNodeSpacingDistance() {return GraphNodeSpacingDistance;}

protected:
	inline static float GraphNodeSpacingDistance = 30.48;
};
