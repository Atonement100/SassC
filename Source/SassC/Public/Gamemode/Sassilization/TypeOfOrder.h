#pragma once

#include "UObject/Class.h"

UENUM(BlueprintType)
enum class ETypeOfOrder : uint8
{
	Order_Unit UMETA(DisplayName = "Unit"),
	Order_Building UMETA(DisplayName = "Building"),
	Order_World UMETA(DisplayName = "World")
};