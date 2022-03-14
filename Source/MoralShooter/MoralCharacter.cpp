// Fill out your copyright notice in the Description page of Project Settings.


#include "MoralCharacter.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FMODBlueprintStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

// Sets default values
AMoralCharacter::AMoralCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FP Camera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->bUsePawnControlRotation = false;

	WeaponSwitcher = CreateDefaultSubobject<UWeaponSwitcher>(TEXT("Weapon Switcher"));
	WeaponSwitcher->SetupAttachment(CameraComponent);

	FoleyFMODComponent = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("FMOD Foley"));
	FoleyFMODComponent->SetupAttachment(GetCapsuleComponent());
	FoleyFMODComponent->SetAutoActivate(false);

	FootstepsFMODComponent = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("FMOD Footsteps"));
	FootstepsFMODComponent->SetupAttachment(GetCapsuleComponent());
	FootstepsFMODComponent->SetAutoActivate(false);
}

// Called when the game starts or when spawned
void AMoralCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentEyeHeight = BaseEyeHeight + GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	CurrentHealth = MaxHealth;
}

// Called every frame
void AMoralCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Set eye height
	CurrentEyeHeight = FMath::FInterpTo(
		CurrentEyeHeight,
		(GetCharacterMovement()->IsCrouching() ? CrouchedEyeHeight : BaseEyeHeight) +
			GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
		DeltaTime,
		CameraEyeHeightTransitionSpeed);

	// Leaning
	TargetLeanLoc = LeanDirection * LeanLocAmount;
	TargetLeanRot = LeanDirection * LeanRotAmount;

	CurrentLeanLoc = FMath::FInterpTo(CurrentLeanLoc, TargetLeanLoc, DeltaTime, LeanSpeed);
	CurrentLeanRot = FMath::FInterpTo(CurrentLeanRot, TargetLeanRot, DeltaTime, LeanSpeed);

	FVector NewCameraLocation = GetCharacterMovement()->GetActorFeetLocation();
	NewCameraLocation += FVector(0, 0, CurrentEyeHeight);
	CameraComponent->SetWorldLocation(NewCameraLocation + CurrentLeanLoc * GetActorRightVector());
	CameraComponent->SetWorldRotation(GetViewRotation() + CameraAnimRotation + FRotator(0, 0, CurrentLeanRot));

	// Update footstep distance
	if (!GetCharacterMovement()->IsFalling())
	{
		DistanceSinceLastFootstep += GetCharacterMovement()->Velocity.Size() * DeltaTime;
	}

	// If needed, perform a footstep
	if (GetCharacterMovement()->IsCrouching())
	{
		if (DistanceSinceLastFootstep >= FootstepCrouchingDistance)
		{
			PlayFootstep(0);
		}
	} else
	{
		if (DistanceSinceLastFootstep >= FootstepWalkingDistance)
		{
			PlayFootstep(0);
		}
	}
}

void AMoralCharacter::PlayFootstep(int jumpState)
{
	DistanceSinceLastFootstep = 0;

	if (jumpState == 1)
	{
		// Jump
		FootstepsFMODComponent->SetParameter("FootstepType", 2);
	} else if (jumpState == 2)
	{
		// Land
		FootstepsFMODComponent->SetParameter("FootstepType", 3);
	} else
	{
		// Set the properties of the footstep cue
		if (GetCharacterMovement()->IsCrouching())
		{
			FootstepsFMODComponent->SetParameter("FootstepType", 1);
		} else {
			FootstepsFMODComponent->SetParameter("FootstepType", 0);
		}
	}

	const EPhysicalSurface Surface = GetFootstepSurface();
	FootstepsFMODComponent->SetParameter("FootstepSurface", Surface);
	
	FootstepsFMODComponent->Play();
}

EPhysicalSurface AMoralCharacter::GetFootstepSurface() const
{
	FHitResult HitResult;
	const FVector TraceStart = GetCharacterMovement()->GetActorFeetLocation() + FVector(0, 0, 10);
	const FVector TraceEnd = TraceStart + FVector(0, 0, -20);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.bReturnPhysicalMaterial = true;
	
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Pawn, CollisionQueryParams))
	{
		return UGameplayStatics::GetSurfaceType(HitResult);
	}

	// If there was no hit, then the player is not standing on anything and should not be playing a footstep
	// Return the default surface
	UE_LOG(LogTemp, Warning, TEXT("GetFootstepSurface failed to find a collision when performing its trace."))
	return SurfaceType_Default;
}

float AMoralCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CurrentHealth -= DamageAmount;
	UE_LOG(LogTemp, Display, TEXT("Health: %f"), CurrentHealth)

	if (CurrentHealth <= 0)
	{
		// Die
		Cast<APlayerController>(Controller)->ConsoleCommand("quit");
	}

	return DamageAmount;
}

void AMoralCharacter::MovementVerticalActionHandler(const FInputActionValue& ActionValue)
{
	MovementInput.X = ActionValue.Get<float>();
	AddMovementInput(GetActorForwardVector(), MovementInput.X);
}

void AMoralCharacter::MovementHorizontalActionHandler(const FInputActionValue& ActionValue)
{
	MovementInput.Y = ActionValue.Get<float>();
	AddMovementInput(GetActorRightVector(), MovementInput.Y);
}

void AMoralCharacter::MovementJumpActionHandler()
{
	Jump();
}

void AMoralCharacter::MovementCrouchActionHandler(const FInputActionValue& ActionValue)
{
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}
	
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	} else
	{
		Crouch();
	}
}

void AMoralCharacter::ViewVerticalActionHandler(const FInputActionValue& ActionValue)
{
	MouseInput.Y = ActionValue.Get<float>();
	AddControllerPitchInput(MouseInput.Y);
}

void AMoralCharacter::ViewHorizontalActionHandler(const FInputActionValue& ActionValue)
{
	MouseInput.X = ActionValue.Get<float>();
	AddControllerYawInput(MouseInput.X);
}

void AMoralCharacter::OnJumped_Implementation()
{
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(JumpCameraShake);

	// Only play jumping footstep if we aren't playing a landing footstep
	float UserValue;
	float FinalValue;
	FootstepsFMODComponent->GetParameterValue("FootstepType", UserValue, FinalValue);
	if (FootstepsFMODComponent->GetTimelinePosition() >= 100.f || FinalValue != 3)
	{
		PlayFootstep(1);
	}
	
	WeaponSwitcher->WeaponActor->OnJumped();
}

void AMoralCharacter::Landed(const FHitResult& Hit)
{
	// The character still has to perform it's internal logic for landing
	Super::Landed(Hit);

	const float LandingSpeed = FMath::Abs(GetCharacterMovement()->Velocity.Z);
	UE_LOG(LogTemp, Display, TEXT("Fell at %f"), LandingSpeed)
	
	const float ShakeIntensity = LandingShakeCurve->FloatCurve.Eval(LandingSpeed);

	UE_LOG(LogTemp, Display, TEXT("Landing shake intensity of %f"), ShakeIntensity)
	
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(LandCameraShake, ShakeIntensity);
	PlayFootstep(2);
	WeaponSwitcher->WeaponActor->OnLanded();
}

void AMoralCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	
	FoleyFMODComponent->Event = CrouchEvent;
	FoleyFMODComponent->Play();

	if (WeaponSwitcher->WeaponActor)
	{
		WeaponSwitcher->WeaponActor->OnCrouchChange(true);
	}

	// In order to prevent footstep spamming when crouching and standing, reset the footstep distance taken
	DistanceSinceLastFootstep = 0;
}

void AMoralCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	FoleyFMODComponent->Event = CrouchEvent;
	FoleyFMODComponent->Play();

	if (WeaponSwitcher->WeaponActor)
	{
		WeaponSwitcher->WeaponActor->OnCrouchChange(false);
	}

	// In order to prevent footstep spamming when crouching and standing, reset the footstep distance taken
	DistanceSinceLastFootstep = 0;
}

void AMoralCharacter::MouseWheelActionHandler(const FInputActionValue& ActionValue)
{
	if (ActionValue.Get<float>() > 0)
	{
		WeaponSwitcher->InputNextWeapon();
	} else
	{
		WeaponSwitcher->InputPreviousWeapon();
	}
}

void AMoralCharacter::LeanActionHandler(const FInputActionValue& ActionValue)
{
	if (LeanDirection == ActionValue.Get<float>())
	{
		LeanDirection = 0;
	} else
	{
		LeanDirection = ActionValue.Get<float>();	
	}
}

void AMoralCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	
	// Make sure that we have a valid PlayerController.
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings.
			Subsystem->ClearAllMappings();

			if (InputBaseMapping)
			{
				Subsystem->AddMappingContext(InputBaseMapping, 0);
			}
		}
	}
}

// Called to bind functionality to input
void AMoralCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Ensure that we are using EnhancedInput
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Vertical Movement
		if (MovementVerticalAction)
		{
			EnhancedInputComponent->BindAction(MovementVerticalAction, ETriggerEvent::Triggered, this,
				&AMoralCharacter::MovementVerticalActionHandler);

			EnhancedInputComponent->BindAction(MovementVerticalAction, ETriggerEvent::Completed, this,
				&AMoralCharacter::MovementVerticalActionHandler);
		}

		// Horizontal Movement
		if (MovementHorizontalAction)
		{
			EnhancedInputComponent->BindAction(MovementHorizontalAction, ETriggerEvent::Triggered, this,
				&AMoralCharacter::MovementHorizontalActionHandler);

			EnhancedInputComponent->BindAction(MovementHorizontalAction, ETriggerEvent::Completed, this,
				&AMoralCharacter::MovementHorizontalActionHandler);
		}

		// Jumping
		if (MovementJumpAction)
		{
			EnhancedInputComponent->BindAction(MovementJumpAction, ETriggerEvent::Triggered, this,
				&AMoralCharacter::MovementJumpActionHandler);
		}

		// Crouching
		if (MovementCrouchAction)
		{
			EnhancedInputComponent->BindAction(MovementCrouchAction, ETriggerEvent::Started, this,
				&AMoralCharacter::MovementCrouchActionHandler);
		}

		// View Vertical
		if (ViewVerticalAction)
		{
			EnhancedInputComponent->BindAction(ViewVerticalAction, ETriggerEvent::Triggered, this,
				&AMoralCharacter::ViewVerticalActionHandler);
		}

		// View Horizontal
		if (ViewHorizontalAction)
		{
			EnhancedInputComponent->BindAction(ViewHorizontalAction, ETriggerEvent::Triggered, this,
				&AMoralCharacter::ViewHorizontalActionHandler);
		}

		// Leaning
		if (LeanAction)
		{
			EnhancedInputComponent->BindAction(LeanAction, ETriggerEvent::Started, this,
				&AMoralCharacter::LeanActionHandler);
		}

		// MouseWheel
		if (MouseWheel)
		{
			EnhancedInputComponent->BindAction(MouseWheel, ETriggerEvent::Triggered, this,
				&AMoralCharacter::MouseWheelActionHandler);
		}

		// Primary Firing
		if (PrimaryFireAction && WeaponSwitcher)
		{
			EnhancedInputComponent->BindAction(PrimaryFireAction, ETriggerEvent::Started, WeaponSwitcher,
				&UWeaponSwitcher::InputPrimaryAction);

			EnhancedInputComponent->BindAction(PrimaryFireAction, ETriggerEvent::Completed, WeaponSwitcher,
				&UWeaponSwitcher::InputPrimaryAction);
		}
		
		// Secondary Firing
		if (SecondaryFireAction && WeaponSwitcher)
		{
			EnhancedInputComponent->BindAction(SecondaryFireAction, ETriggerEvent::Triggered, WeaponSwitcher,
				&UWeaponSwitcher::InputSecondaryAction);
		}
		
		// Reload
		if (ReloadAction && WeaponSwitcher)
		{
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, WeaponSwitcher,
				&UWeaponSwitcher::InputReload);
		}
	}
}