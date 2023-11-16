#pragma once

//子弹类型enum
UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	Eat_9MM UMETA(DisplayName = "9mm"),
	Eat_AR UMETA(DisplayName = "AssultRifle"),

	Eat_Max UMETA(DisplayName = "DefaultMax"),
};