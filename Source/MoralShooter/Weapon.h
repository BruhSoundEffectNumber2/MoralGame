// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FMODAudioComponent.h"
#include "InputActionValue.h"
#include "WeaponState.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

// Forward declarations
class AMoralCharacter;

/* The base class that all weapon's inherit from. */
UCLASS()
class MORALSHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	/** The mesh that a weapon uses for animation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* WeaponMesh;

	/** The mesh for the arms. Uses anim data from weapon mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* ArmMesh;

	/** Audio component for character foley. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UFMODAudioComponent* FoleyFMODComponent;


	/** Animation Montage to play for drawing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Animation")
	UAnimMontage* DrawMontage;

	/** Animation Montage to play for holstering. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Animation")
	UAnimMontage* HolsterMontage;

	/** Animation Montage to play for jumping. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Animation")
	UAnimMontage* JumpingMontage;

	/** Animation Montage to play for landing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Animation")
	UAnimMontage* LandingMontage;
	

	/** How fast the weapon moves when it's swaying. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Sway")
	float SwaySpeed;

	/** Multiplier for the weapon's location swaying. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Sway")
	FVector SwayLocMultiplier;

	/** Multiplier for the weapon's rotation swaying. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Sway")
	FRotator SwayRotMultiplier;

	/** Maximum amount the weapon's location can sway. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Sway")
	FVector SwayLocLimit;

	/** Maximum amount the weapon's rotation can sway. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Sway")
	FRotator SwayRotLimit;


	/** How quickly the weapon's motion starts/stops. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Movement")
	float MovementTransitionSpeed;

	/** How quickly the weapon's directional offset changes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Movement")
	float MovementDirectionTransitionSpeed;

	/** The multiplier applied to the period of the weapon movement while the player is crouched. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Movement")
	float MovementPeriodCrouchedMultiplier;
	
	/** How far the weapon moves in each direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Movement|Location")
	FVector MovementLocationAmplitude;

	/** How fast the weapon moves in each direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Movement|Location")
	FVector MovementLocationPeriod;

	/** Offsets the weapons location period to start at different points. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Movement|Location")
	FVector MovementLocationTimeOffset;

	/** Offsets the weapons location in each direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Movement|Location")
	FVector MovementLocationOffset;

	/** How much the weapon moves in the direction the player is walking. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Movement|Location")
	FVector MovementDirectionLocationAmount;

	/** How far the weapon rotates in each direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Movement|Rotation")
	FRotator MovementRotationAmplitude;

	/** How fast the weapon rotates in each direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Movement|Rotation")
	FRotator MovementRotationPeriod;

	/** Offsets the weapons rotation period to start at different points. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Movement|Rotation")
	FRotator MovementRotationTimeOffset;

	/** Offsets the weapons rotation in each direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Movement|Rotation")
	FRotator MovementRotationOffset;

	/** How much the weapon rotates in the direction the player is walking. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Movement|Rotation")
	FRotator MovementDirectionRotationAmount;
	
	
	/** How much the weapon will tilt in the direction the player is leaning. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Movement|Lean")
	float LeanRotation;

	/** How fast the weapon will tilt when the player is leaning. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Movement|Lean")
	float LeanSpeed;
	

	/** An offset applied to the location of the weapon when crouching. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Crouch")
	FVector CrouchOffset;

	/** Transition speed for crouch offset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Crouch")
	float CrouchOffsetTransitionSpeed;


	/** Sound for crouching foley */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Audio")
	UFMODEvent* CrouchFoleyEvent;
	
	UPROPERTY()
	UWeaponState* State;

	UPROPERTY()
	AMoralCharacter* CharacterOwner;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	FTimerHandle DrawTimerHandle;

	UPROPERTY()
	FTransform CurrentSwayTransform = FTransform(FVector(0));

	UPROPERTY()
	FTransform TargetSwayTransform = FTransform(FVector(0));

	UPROPERTY()
	FTransform MovementTransform = FTransform(FVector(0));

	UPROPERTY()
	FTransform MovementDirectionTransform = FTransform(FVector(0));

	float CurrentLeanRotation;
	float TargetLeanRotation;

	UPROPERTY()
	float CurrentMovementAlpha;

	UPROPERTY()
	float CurrentCrouchOffsetAlpha;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Initializes values needed by the weapon from the WeaponSwitcher. */
	virtual void Init(UWeaponState* StatePointer, bool bFirst);

	/** Draws the weapon into a firing position. */
	virtual void Draw();

	/** Moves the weapon into it's holster. */
	virtual void Holster();

	/** For the weapons primary action (ie. firing a bullet on a trigger press) */
	virtual void InputPrimaryAction(const FInputActionValue& ActionValue);

	/** For the weapons secondary action (ie. aiming down the sight of a sniper rifle) */
	virtual void InputSecondaryAction(const FInputActionValue& ActionValue);

	UFUNCTION()
	void DrawTimerComplete();

	UFUNCTION()
	void HolsterTimerComplete();

	void OnCrouchChange(bool bStartCrouching);

	void OnJumped();

	void OnLanded();
};
