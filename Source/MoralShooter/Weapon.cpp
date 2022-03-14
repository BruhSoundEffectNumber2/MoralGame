// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "MoralCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	RootComponent = WeaponMesh;

	ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arm Mesh"));
	ArmMesh->SetupAttachment(RootComponent);
	ArmMesh->SetMasterPoseComponent(WeaponMesh);

	FoleyFMODComponent = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("FMOD Foley"));
	FoleyFMODComponent->SetupAttachment(WeaponMesh, TEXT("Weapon"));
	FoleyFMODComponent->SetAutoActivate(false);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Weapon Sway
	{
		const FVector2D MouseInput = CharacterOwner->GetMouseInput();
	
		// Location
		FVector TargetSwayLoc = FVector(0);
	
		TargetSwayLoc.Y = FMath::Clamp(
			MouseInput.X * SwayLocMultiplier.Y,
			-SwayLocLimit.Y,
			SwayLocLimit.Y);
	
		TargetSwayLoc.Z = FMath::Clamp(
			-MouseInput.Y * SwayLocMultiplier.Y,
			-SwayLocLimit.Z,
			SwayLocLimit.Z);

		TargetSwayTransform.SetLocation(TargetSwayLoc);
	
		CurrentSwayTransform.SetLocation(FMath::InterpEaseIn(
			CurrentSwayTransform.GetLocation(),
			TargetSwayTransform.GetLocation(),
			GetWorld()->GetDeltaSeconds() * SwaySpeed,
			1.1f));

		// Rotation
		FRotator TargetSwayRot = FRotator(0);
	
		TargetSwayRot.Pitch = FMath::Clamp(
			-MouseInput.Y * SwayRotMultiplier.Pitch,
			-SwayRotLimit.Pitch,
			SwayRotLimit.Pitch);
	
		TargetSwayRot.Yaw = FMath::Clamp(
			MouseInput.X * SwayRotMultiplier.Yaw,
			-SwayRotLimit.Yaw,
			SwayRotLimit.Yaw);

		TargetSwayTransform.SetRotation(TargetSwayRot.Quaternion());
	
		CurrentSwayTransform.SetRotation(FMath::InterpEaseIn(
			CurrentSwayTransform.GetRotation().Rotator(),
			TargetSwayTransform.GetRotation().Rotator(),
			GetWorld()->GetDeltaSeconds() * SwaySpeed,
			1.1f).Quaternion());
	}

	// Weapon movement
	{
		if (CharacterOwner->GetCharacterMovement()->Velocity.Size2D() <= 0.01f ||
			CharacterOwner->GetCharacterMovement()->IsFalling())
		{
			CurrentMovementAlpha = FMath::Lerp(CurrentMovementAlpha, 0.f,
				MovementTransitionSpeed * GetWorld()->GetDeltaSeconds());
		} else
		{
			CurrentMovementAlpha = FMath::Lerp(CurrentMovementAlpha, 1.f,
				MovementTransitionSpeed * GetWorld()->GetDeltaSeconds());
		}

		const float CurrentMovementPeriodMultiplier = CharacterOwner->bIsCrouched ? MovementPeriodCrouchedMultiplier : 1.f;
		
		FVector Location;
		
		Location.X =
			FMath::Sin(GetWorld()->GetTimeSeconds() * MovementLocationPeriod.X * PI * CurrentMovementPeriodMultiplier +
				MovementLocationTimeOffset.X) *
				MovementLocationAmplitude.X + MovementLocationOffset.X;

		Location.Y =
			FMath::Sin(GetWorld()->GetTimeSeconds() * MovementLocationPeriod.Y * PI * CurrentMovementPeriodMultiplier +
				MovementLocationTimeOffset.Y) *
				MovementLocationAmplitude.Y + MovementLocationOffset.Y;

		Location.Z =
			FMath::Sin(GetWorld()->GetTimeSeconds() * MovementLocationPeriod.Z * PI * CurrentMovementPeriodMultiplier +
				MovementLocationTimeOffset.Z) *
				MovementLocationAmplitude.Z + MovementLocationOffset.Z;

		const FVector NewLocation = FMath::Lerp(FVector(0), Location,
			CurrentMovementAlpha);

		FRotator Rotation;
		
		Rotation.Pitch =
			FMath::Sin(GetWorld()->GetTimeSeconds() * MovementRotationPeriod.Pitch * PI *
				CurrentMovementPeriodMultiplier + MovementRotationTimeOffset.Pitch) *
				MovementRotationAmplitude.Pitch + MovementRotationOffset.Pitch;

		Rotation.Yaw =
			FMath::Sin(GetWorld()->GetTimeSeconds() * MovementRotationPeriod.Yaw * PI *
				CurrentMovementPeriodMultiplier + MovementRotationTimeOffset.Yaw) *
				MovementRotationAmplitude.Yaw + MovementRotationOffset.Yaw;

		Rotation.Roll =
			FMath::Sin(GetWorld()->GetTimeSeconds() * MovementRotationPeriod.Roll * PI *
				CurrentMovementPeriodMultiplier + MovementRotationTimeOffset.Roll) *
				MovementRotationAmplitude.Roll + MovementRotationOffset.Roll;

		const FRotator NewRotation = FMath::Lerp(FRotator(0), Rotation,
			CurrentMovementAlpha);

		// Direction
		const FVector2D MovementInput = CharacterOwner->GetMovementInput();
		
		FVector DirectionLocation;

		DirectionLocation.X = MovementInput.X * MovementDirectionLocationAmount.X;
		DirectionLocation.Y = MovementInput.Y * MovementDirectionLocationAmount.Y;

		const FVector NewDirectionLocation = FMath::Lerp(
			MovementDirectionTransform.GetLocation(),
			DirectionLocation,
			GetWorld()->GetDeltaSeconds() * MovementDirectionTransitionSpeed);

		MovementDirectionTransform.SetLocation(NewDirectionLocation);

		FRotator DirectionRotation;

		DirectionRotation.Pitch = -MovementInput.X * MovementDirectionRotationAmount.Pitch;
		DirectionRotation.Roll = MovementInput.Y * MovementDirectionRotationAmount.Roll;

		const FRotator NewDirectionRotation = FMath::Lerp(
			MovementDirectionTransform.GetRotation().Rotator(),
			DirectionRotation,
			GetWorld()->GetDeltaSeconds() * MovementDirectionTransitionSpeed);

		MovementDirectionTransform.SetRotation(NewDirectionRotation.Quaternion());

		// Crouch offset
		CurrentCrouchOffsetAlpha = FMath::FInterpTo(
			CurrentCrouchOffsetAlpha,
			CharacterOwner->bIsCrouched ? 1.f : 0.f,
			GetWorld()->GetDeltaSeconds(),
			CrouchOffsetTransitionSpeed);

		const FVector CurrentCrouchOffset = FMath::Lerp(FVector(0), CrouchOffset, CurrentCrouchOffsetAlpha);

		// Leaning
		TargetLeanRotation = CharacterOwner->GetLeanDirection() * LeanRotation;
		CurrentLeanRotation = FMath::FInterpTo(CurrentLeanRotation, TargetLeanRotation,
			GetWorld()->GetDeltaSeconds(), LeanSpeed);

		MovementTransform.SetLocation(NewLocation + NewDirectionLocation + CurrentCrouchOffset);
		MovementTransform.SetRotation((
			NewRotation + NewDirectionRotation + FRotator(0, 0, CurrentLeanRotation)
			).Quaternion());

		// Logging the transform fixes an issue where the weapon will slowly spin out over the player, don't know why
		UE_LOG(LogTemp, VeryVerbose, TEXT("%s"), *MovementTransform.ToHumanReadableString())
	}
}

void AWeapon::Draw()
{
	State->WeaponAction = EWeaponAction::Draw;

	float MontageLength = 0;
	if (WeaponMesh && DrawMontage)
	{
		MontageLength = WeaponMesh->GetAnimInstance()->Montage_Play(DrawMontage);
	}

	GetWorldTimerManager().SetTimer(
		DrawTimerHandle,
		this,
		&AWeapon::DrawTimerComplete,
		MontageLength
	);
}

void AWeapon::DrawTimerComplete()
{
	State->WeaponAction = EWeaponAction::Idle;
}

void AWeapon::Holster()
{
	State->WeaponAction = EWeaponAction::Holster;

	float MontageLength = 0;
	if (WeaponMesh && HolsterMontage)
	{
		MontageLength = WeaponMesh->GetAnimInstance()->Montage_Play(HolsterMontage);
	}

	GetWorldTimerManager().SetTimer(
		DrawTimerHandle,
		this,
		&AWeapon::HolsterTimerComplete,
		MontageLength
	);
}

void AWeapon::HolsterTimerComplete()
{
	State->WeaponAction = EWeaponAction::Idle;
	CharacterOwner->WeaponSwitcher->ChangeWeaponActor();
}

void AWeapon::InputPrimaryAction(const FInputActionValue& ActionValue)
{
	
}

void AWeapon::InputSecondaryAction(const FInputActionValue& ActionValue)
{
	
}

void AWeapon::OnCrouchChange(bool bStartCrouching)
{
	FoleyFMODComponent->Event = CrouchFoleyEvent;
	FoleyFMODComponent->Play();
}

void AWeapon::OnJumped()
{
	WeaponMesh->GetAnimInstance()->Montage_Stop(0, LandingMontage);
	WeaponMesh->GetAnimInstance()->Montage_Play(
		JumpingMontage,
		1,
		EMontagePlayReturnType::MontageLength,
		0,
		false);
}

void AWeapon::OnLanded()
{
	WeaponMesh->GetAnimInstance()->Montage_Play(
		LandingMontage,
		1,
		EMontagePlayReturnType::MontageLength,
		0,
		false);
}

void AWeapon::Init(UWeaponState* StatePointer, bool bFirst)
{
	State = StatePointer;
	if (bFirst)
	{
		State->bInitialized = true;
	}
}
