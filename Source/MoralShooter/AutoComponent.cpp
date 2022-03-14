// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoComponent.h"

#include "FMODBlueprintStatics.h"
#include "Gun.h"
#include "NiagaraFunctionLibrary.h"

UAutoComponent::UAutoComponent()
{
	Behavior = EGunComponentBehavior::Fire;	
}

void UAutoComponent::Fire(bool Start)
{
	bWantsFire = Start;
}

void UAutoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (bWantsFire && Owner && Owner->State)
	{
		if (Owner->State->WeaponAction == EWeaponAction::Idle &&
			Cast<UGunState>(Owner->State)->GunAction == EGunAction::None &&
			Cast<UGunState>(Owner->State)->CurrentAmmo > 0)
		{
			ExecFire();
		}
	}	
}

void UAutoComponent::ExecFire()
{
	Owner->State->WeaponAction = EWeaponAction::Other;
	Cast<UGunState>(Owner->State)->GunAction = EGunAction::Fire;
	
	if (Owner->AnimInstance && Owner->FireMontage)
	{
		Owner->AnimInstance->Montage_Play(Owner->FireMontage,
			1,
			EMontagePlayReturnType::MontageLength,
			0,
			true);
	}

	Owner->GetWorldTimerManager().SetTimer
	(
			TimerHandle,
			this,
			&UAutoComponent::FireTimerComplete,
			1.f / (Owner->FireRate / 60.f)
	);

	UFMODBlueprintStatics::PlayEventAttached(
		Owner->FireFMODComponent->Event,
		Owner->WeaponMesh,
		"Muzzle",
		FVector(0),
		EAttachLocation::KeepRelativeOffset,
		false, true, true);

	// Muzzle flash
	UNiagaraFunctionLibrary::SpawnSystemAttached(
		Owner->MuzzleFlashSystem,
		Owner->WeaponMesh,
		"Muzzle",
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true,
		true,
		ENCPoolMethod::AutoRelease);

	// Ammo eject
	UNiagaraFunctionLibrary::SpawnSystemAttached(
		Owner->AmmoEjectSystem,
		Owner->WeaponMesh,
		"Ejection",
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true,
		true,
		ENCPoolMethod::AutoRelease);

	Owner->ShootBullet();
	Owner->Recoil();

	Cast<UGunState>(Owner->State)->CurrentAmmo--;
}

void UAutoComponent::FireTimerComplete() const
{
	Owner->State->WeaponAction = EWeaponAction::Idle;
	Cast<UGunState>(Owner->State)->GunAction = EGunAction::None;	
}