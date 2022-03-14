// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WeaponState.generated.h"

class AWeapon;

/* Defines the actions a weapon can perform. */
UENUM(BlueprintType)
enum class EWeaponAction : uint8
{
	Idle,
	Draw,
	Holster,
	Other
};

/* Defines the base state that all weapon's share. */
UCLASS(BlueprintType)
class MORALSHOOTER_API UWeaponState : public UObject
{
	GENERATED_BODY()

public:
	TSubclassOf<AWeapon> Class;

	bool bInitialized;
	
	/* What action the weapon is performing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponAction WeaponAction;
};
