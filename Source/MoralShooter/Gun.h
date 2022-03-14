// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Bullet.h"
#include "FMODAudioComponent.h"
#include "GunComponent.h"
#include "GunState.h"
#include "Weapon.h"
#include "Sound/SoundCue.h"
#include "NiagaraSystem.h"
#include "WeaponAnimInstance.h"
#include "Gun.generated.h"

/* A gun is a form of weapon that fires projectiles. */
UCLASS()
class MORALSHOOTER_API AGun : public AWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGun();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UFMODAudioComponent* FireFMODComponent;
	

	UPROPERTY()
	UWeaponAnimInstance* AnimInstance;
	
	
	/** How many rounds per minute will the gun fire. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|General")
	int32 FireRate;

	/** Bullet class to use. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|General")
	TSubclassOf<ABullet> BulletClass;


	/** Niagara system to use for the muzzle flash. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|FX")
	UNiagaraSystem* MuzzleFlashSystem;
	
	/** Niagara system to use for the ammo ejection. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|FX")
    UNiagaraSystem* AmmoEjectSystem;
	

	/** CameraShake to use for gunshots. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Recoil|Camera")
	TSubclassOf<UCameraShakeBase> GunShotCameraShake;


	/** The direction the camera travels when applying recoil. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Recoil|Camera")
	FVector CameraRecoilDirection;

	/** The max variance for the camera travel direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Recoil|Camera")
	FVector CameraRecoilDirectionVariance;

	/** The min distance to travel in the travel direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Recoil|Camera")
	float CameraRecoilMinTravel;

	/** The max distance to travel in the travel direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Recoil|Camera")
	float CameraRecoilMaxTravel;

	/** The speed that the weapon moves to its recoil position. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Recoil|Weapon")
	float WeaponRecoilSpeed;

	/** The maximum distance the weapon mesh can travel when in recoil. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Recoil|Weapon")
	FVector WeaponRecoilMaxDist;

	/** The maximum rotation the weapon mesh can have when in recoil. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Recoil|Weapon")
	FRotator WeaponRecoilMaxRot;

	/** The minimum movement the weapon can recoil. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Recoil|Weapon")
	FVector WeaponRecoilMinLoc;

	/** The maximum movement the weapon can recoil. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Recoil|Weapon")
	FVector WeaponRecoilMaxLoc;

	/** The minimum rotation the weapon can recoil. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Recoil|Weapon")
	FRotator WeaponRecoilMinRotation;

	/** The maximum rotation the weapon can recoil. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Recoil|Weapon")
	FRotator WeaponRecoilMaxRotation;
	

	/** How far a bullet can travel and still do damage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Ballistics")
	int32 Range;

	/** The variance a bullet can take from the forward direction while idle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Ballistics|Accuracy", META=(ClampMin=0.f))
	float BulletBaseVariance;

	/** The variance a bullet can take from the forward direction while moving. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Ballistics|Accuracy", META=(ClampMin=0.f))
	float BulletWalkingVariance;
	
	/** The variance a bullet can take from the forward direction while falling. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Ballistics|Accuracy", META=(ClampMin=0.f))
	float BulletFallingVariance;

	/** How much damage a bullet will do to the torso after travelling 0 meters. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Ballistics")
	int32 TorsoDamage;
	
	/** Influences how much the damage of a bullet decreases with distance. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Ballistics", META=(ClampMin=0.f, ClampMax=1.f))
	float DamageDropOffFactor;
	
	
	/** Animation Montage to play for firing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Animation")
	UAnimMontage* FireMontage;
	
	/** Animation Montage to play for reloading. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Animation")
	UAnimMontage* ReloadMontage;
	

	/** How many rounds can the gun hold in a magazine. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Ammo")
	int32 MagazineAmmo;

	/** How many rounds can the gun hold in reserve. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Ammo")
	int32 MaxReserveAmmo;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	UGunComponent* FireComponent;

	UPROPERTY()
	UGunComponent* ReloadComponent;

	FTimerHandle ActionTimerHandle;

	bool bPrimaryFirePressed = false;

	FTransform WeaponRecoilTransform;

	FTransform CurrentWeaponRecoilTransform;

public:
	virtual void Init(UWeaponState* StatePointer, bool bFirst) override;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void InputPrimaryAction(const FInputActionValue& ActionValue) override;

	virtual void InputSecondaryAction(const FInputActionValue& ActionValue) override;

	virtual void InputReloadAction(const FInputActionValue& ActionValue);

	void ShootBullet() const;

	void Recoil();

	float CalculateBulletVariance() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentBulletVariance() const
	{
		return CalculateBulletVariance();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentAmmo() const
	{
		return Cast<UGunState>(State)->CurrentAmmo;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetReserveAmmo() const
	{
		return Cast<UGunState>(State)->ReserveAmmo;
	}
};
