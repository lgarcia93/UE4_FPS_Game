// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */

UENUM(BlueprintType)
enum class EWeaponType : uint8 {
	Pistol = 0,
	Shotgun = 1,
	SMG = 2,
	Rifle = 3,
	PrecisionRifle = 4,
	RocketLauncher = 5,
	GrenadeLauncher = 6
};

UENUM(BlueprintType)
enum class EShellCasingType : uint8
{
	Pistol = 0,
	Shotgun = 1,
	SMG = 2,
	Rifle = 3,
	RocketLauncher = 4,
	GrenadeLauncher = 5
};

UENUM(BlueprintType)
enum class ETeam : uint8 {
	TeamA = 0,
	TeamB = 1,
	None = 2
};


UENUM(BlueprintType)
enum class EWeaponFiringMode : uint8 {
	SingleShot = 0,
	Burst2 = 1,
	Burst3 = 2,
	FullAuto = 3
};

UENUM(BlueprintType)
enum class EEquipable : uint8 {
	Unnarmed = 0,
	AK74 = 1,
	M16 = 2,
	M67Grenade = 3,
	M9Pistol = 4,
};

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	Gun = 0,
	Grenade = 1,
};