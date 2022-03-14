// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfLoadMagComponent.h"

#include "Gun.h"
#include "GunState.h"

USelfLoadMagComponent::USelfLoadMagComponent()
{
	Behavior = EGunComponentBehavior::Reload;
}

void USelfLoadMagComponent::Reload()
{
	if (
		Cast<UGunState>(Owner->State)->ReserveAmmo == 0 ||
		Cast<UGunState>(Owner->State)->CurrentAmmo == Owner->MagazineAmmo ||
		Cast<UGunState>(Owner->State)->GunAction != EGunAction::None)
	{
		// We cannot reload now
		return;
	}

	Owner->State->WeaponAction = EWeaponAction::Other;
	Cast<UGunState>(Owner->State)->GunAction = EGunAction::Reload;

	float MontageLength = 0;
	if (Owner->AnimInstance && Owner->ReloadMontage)
	{
		MontageLength = Owner->AnimInstance->Montage_Play(Owner->ReloadMontage);
	}

	Owner->GetWorldTimerManager().SetTimer
		(
			TimerHandle,
			this,
			&USelfLoadMagComponent::ReloadTimerComplete,
			MontageLength
	);

	Cast<UGunState>(Owner->State)->ReserveAmmo += Cast<UGunState>(Owner->State)->CurrentAmmo;
	Cast<UGunState>(Owner->State)->CurrentAmmo = 0;
}

void USelfLoadMagComponent::ReloadTimerComplete() const
{
	if (Cast<UGunState>(Owner->State)->ReserveAmmo >= Owner->MagazineAmmo)
	{
		Cast<UGunState>(Owner->State)->ReserveAmmo -= Owner->MagazineAmmo;
		Cast<UGunState>(Owner->State)->CurrentAmmo += Owner->MagazineAmmo;
	} else
	{
		Cast<UGunState>(Owner->State)->CurrentAmmo += Cast<UGunState>(Owner->State)->ReserveAmmo;
		Cast<UGunState>(Owner->State)->ReserveAmmo = 0;
	}

	Owner->State->WeaponAction = EWeaponAction::Idle;
	Cast<UGunState>(Owner->State)->GunAction = EGunAction::None;
}
