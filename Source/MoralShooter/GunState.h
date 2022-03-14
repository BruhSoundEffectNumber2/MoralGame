// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponState.h"
#include "GunState.generated.h"

/* Defines the actions a gun can perform. */
UENUM(BlueprintType)
enum class EGunAction : uint8
{
	None,
	Fire,
	Reload,
	Melee
};

/* Defines the base state that all gun's share. */
UCLASS(BlueprintType)
class MORALSHOOTER_API UGunState : public UWeaponState
{
	GENERATED_BODY()

public:
	/* What action the gun is performing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGunAction GunAction;
	
	/* How much ammo the gun has. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentAmmo;

	/* How much ammo the gun has in reserve. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ReserveAmmo;
	
};
