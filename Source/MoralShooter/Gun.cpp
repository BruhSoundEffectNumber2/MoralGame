// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "MoralCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AGun::AGun()
{
	FireFMODComponent = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("FMOD Gunshots"));
	FireFMODComponent->SetupAttachment(RootComponent, TEXT("Weapon"));
	FireFMODComponent->SetAutoActivate(false);
}

void AGun::BeginPlay()
{
	Super::BeginPlay();
	
	AnimInstance = Cast<UWeaponAnimInstance>(WeaponMesh->GetAnimInstance());
}

void AGun::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	AnimInstance->State = State;

	WeaponRecoilTransform.SetLocation(
		FMath::InterpEaseIn(WeaponRecoilTransform.GetLocation(), FVector::ZeroVector,
			WeaponRecoilSpeed * DeltaSeconds, 1.1f));

	WeaponRecoilTransform.SetRotation(
		FMath::InterpEaseIn(WeaponRecoilTransform.GetRotation().Rotator(), FRotator::ZeroRotator,
			WeaponRecoilSpeed * DeltaSeconds, 1.1f).Quaternion());

	const FVector FinalLocationOffset =
		WeaponRecoilTransform.GetLocation() +
			CurrentSwayTransform.GetLocation() +
				MovementTransform.GetLocation();

	const FQuat FinalRotationOffset =
		WeaponRecoilTransform.GetRotation() +
			CurrentSwayTransform.GetRotation() +
				MovementTransform.GetRotation();
	
	AnimInstance->OffsetTransform.SetLocation(FinalLocationOffset);
	AnimInstance->OffsetTransform.SetRotation(FinalRotationOffset);
}

void AGun::ShootBullet() const
{
	ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(BulletClass);
	Bullet->SetActorLocation(WeaponMesh->GetSocketLocation("Muzzle"));

	FRotator BulletRotation = CharacterOwner->CameraComponent->GetComponentRotation();

	// Add variance to the bullets
	const float Variance = CalculateBulletVariance();
	
	BulletRotation += FRotator(
		FMath::FRandRange(-Variance, Variance),
		FMath::FRandRange(-Variance, Variance),
		FMath::FRandRange(-Variance, Variance));
	
	Bullet->SetActorRotation(BulletRotation);
	Bullet->Init(DamageDropOffFactor, TorsoDamage, Range, CharacterOwner->GetController());
}

float AGun::CalculateBulletVariance() const
{
	float Variance;

	if (CharacterOwner->GetCharacterMovement()->IsFalling())
	{
		Variance = BulletFallingVariance;
	} else
	{
		if (CharacterOwner->GetVelocity().IsNearlyZero())
		{
			Variance = BulletBaseVariance;
		} else
		{
			Variance = BulletWalkingVariance;
		}
	}

	return Variance;
}

void AGun::Recoil()
{
	const FVector RecoilDir = CameraRecoilDirection + FVector(
		FMath::FRandRange(-CameraRecoilDirectionVariance.X, CameraRecoilDirectionVariance.X),
		FMath::FRandRange(-CameraRecoilDirectionVariance.Y, CameraRecoilDirectionVariance.Y),
		0);

	FRotator NewCameraRot = CharacterOwner->Controller->GetControlRotation();
	NewCameraRot += FRotator(
		RecoilDir.Y * FMath::FRandRange(CameraRecoilMinTravel, CameraRecoilMaxTravel),
		RecoilDir.X * FMath::FRandRange(CameraRecoilMinTravel, CameraRecoilMaxTravel),
		0);

	CharacterOwner->Controller->SetControlRotation(NewCameraRot);

	// Weapon location
	FVector NewWeaponLoc = WeaponRecoilTransform.GetLocation();
	
	NewWeaponLoc += FVector(
		FMath::FRandRange(WeaponRecoilMinLoc.X, WeaponRecoilMaxLoc.X),
		FMath::FRandRange(WeaponRecoilMinLoc.Y, WeaponRecoilMaxLoc.Y),
		FMath::FRandRange(WeaponRecoilMinLoc.Z, WeaponRecoilMaxLoc.Z));
	
	NewWeaponLoc.X = FMath::Clamp(NewWeaponLoc.X, -WeaponRecoilMaxDist.X, WeaponRecoilMaxDist.X);
	NewWeaponLoc.Y = FMath::Clamp(NewWeaponLoc.Y, -WeaponRecoilMaxDist.Y, WeaponRecoilMaxDist.Y);
	NewWeaponLoc.Z = FMath::Clamp(NewWeaponLoc.Z, -WeaponRecoilMaxDist.Z, WeaponRecoilMaxDist.Z);

	WeaponRecoilTransform.SetLocation(NewWeaponLoc);

	// Weapon rotation
	FRotator NewWeaponRot = WeaponRecoilTransform.GetRotation().Rotator();
	
	NewWeaponRot += FRotator(
		FMath::FRandRange(WeaponRecoilMinRotation.Pitch, WeaponRecoilMaxRotation.Pitch),
		FMath::FRandRange(WeaponRecoilMinRotation.Yaw, WeaponRecoilMaxRotation.Yaw),
		FMath::FRandRange(WeaponRecoilMinRotation.Roll, WeaponRecoilMaxRotation.Roll));
	
	NewWeaponRot.Pitch = FMath::Clamp(NewWeaponRot.Pitch, -WeaponRecoilMaxRot.Pitch, WeaponRecoilMaxRot.Pitch);
	NewWeaponRot.Yaw = FMath::Clamp(NewWeaponRot.Yaw, -WeaponRecoilMaxRot.Yaw, WeaponRecoilMaxRot.Yaw);
	NewWeaponRot.Roll = FMath::Clamp(NewWeaponRot.Roll, -WeaponRecoilMaxRot.Roll, WeaponRecoilMaxRot.Roll);

	WeaponRecoilTransform.SetRotation(NewWeaponRot.Quaternion());

	// Camera shake
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(
		GunShotCameraShake);
}

void AGun::InputPrimaryAction(const FInputActionValue& ActionValue)
{
	Super::InputPrimaryAction(ActionValue);
	
	if (FireComponent)
	{
		FireComponent->Fire(ActionValue.Get<bool>());
	}
}

void AGun::InputSecondaryAction(const FInputActionValue& ActionValue)
{
	Super::InputSecondaryAction(ActionValue);
}

void AGun::InputReloadAction(const FInputActionValue& ActionValue)
{
	if (ReloadComponent)
	{
		ReloadComponent->Reload();
	}
}

void AGun::Init(UWeaponState* StatePointer, bool bFirst)
{
	Super::Init(StatePointer, bFirst);
	
	// Find and assign GunComponents
	TArray<UGunComponent*, TInlineAllocator<2>> GunComponents;
	GetComponents<UGunComponent>(GunComponents);

	for (UGunComponent* Component : GunComponents)
	{
		switch (Component->Behavior)
		{
			case EGunComponentBehavior::Fire:
				FireComponent = Component;
				break;
			case EGunComponentBehavior::Reload:
				ReloadComponent = Component;
				break;
		}

		Component->Init(this);
	}

	if (bFirst)
	{
		Cast<UGunState>(State)->ReserveAmmo = MaxReserveAmmo;
		Cast<UGunState>(State)->CurrentAmmo = MagazineAmmo;
	}
}
