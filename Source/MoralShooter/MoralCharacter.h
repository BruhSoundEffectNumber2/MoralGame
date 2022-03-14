// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "FMODAudioComponent.h"
#include "InputMappingContext.h"
#include "WeaponSwitcher.h"
#include "Camera/CameraComponent.h"
#include "FMODEvent.h"
#include "MoralCharacter.generated.h"

UCLASS()
class MORALSHOOTER_API AMoralCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMoralCharacter();
	
	
	/** The camera that the player views the world through. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* CameraComponent;

	/** Holds the weapons and handles giving input and switching between them. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWeaponSwitcher* WeaponSwitcher;

	/** Audio component for character foley. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UFMODAudioComponent* FoleyFMODComponent;

	/** Audio component for character footsteps. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UFMODAudioComponent* FootstepsFMODComponent;
	

	/** InputAction for vertical player movement (WS). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Input")
	UInputAction* MovementVerticalAction;
	
	/** InputAction for horizontal player movement (AD). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Input")
	UInputAction* MovementHorizontalAction;

	/** InputAction for jumping. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Input")
	UInputAction* MovementJumpAction;

	/** InputAction for crouching */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Input")
	UInputAction* MovementCrouchAction;

	/** InputAction for the Mouse Y. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Input")
	UInputAction* ViewVerticalAction;

	/** InputAction for the Mouse X. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Input")
	UInputAction* ViewHorizontalAction;

	/** InputAction for primary firing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Input")
	UInputAction* PrimaryFireAction;
	
	/** InputAction for secondary firing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Input")
	UInputAction* SecondaryFireAction;

	/** InputAction for reloading. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Input")
	UInputAction* ReloadAction;

	/** InputAction for MouseWheel. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Input")
	UInputAction* MouseWheel;

	/** InputAction for leaning. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Input")
	UInputAction* LeanAction;
	
	/** InputMappingContext for base InputActions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Input")
	UInputMappingContext* InputBaseMapping;

	
	/** Camera shake for jumping. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Shake")
	TSubclassOf<UCameraShakeBase> JumpCameraShake;

	/** Camera shake for landing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Shake")
	TSubclassOf<UCameraShakeBase> LandCameraShake;

	/** The curve for determining the landing shake intensity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Shake")
	UCurveFloat* LandingShakeCurve;
	

	/** The speed that the camera will move to be at the correct eye height. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Crouch")
	float CameraEyeHeightTransitionSpeed;


	/** How much the player's rotation leans in either direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Leaning")
	float LeanRotAmount;

	/** How much the player's location leans in either direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Leaning")
	float LeanLocAmount;

	/** How fast the player leans. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Leaning")
	float LeanSpeed;
	

	/** Distance between walking footsteps. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Footsteps")
	float FootstepWalkingDistance;

	/** Distance between crouching footsteps. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Footsteps")
	float FootstepCrouchingDistance;


	/** Audio for crouching. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Audio")
	UFMODEvent* CrouchEvent;
	

	/** The max amount of health the player can have. Also the amount of health the player starts with. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Health")
	float MaxHealth = 100;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float CurrentEyeHeight;

	float DistanceSinceLastFootstep;

	/** The current camera animation rotation. */
	UPROPERTY()
	FRotator CameraAnimRotation;

	/** The current mouse input. */
	UPROPERTY()
	FVector2D MouseInput;

	/** The current movement input. */
	UPROPERTY()
	FVector2D MovementInput;

	int LeanDirection;

	float CurrentLeanLoc;
	float TargetLeanLoc;
	float CurrentLeanRot;
	float TargetLeanRot;

	virtual void OnJumped_Implementation() override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	void PlayFootstep(int jumpState);

	EPhysicalSurface GetFootstepSurface() const;

	float CurrentHealth;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**
	 * Applies input from MovementVerticalAction to the Character.
	 * @param ActionValue The Input Action's value.
	 */
	void MovementVerticalActionHandler(const FInputActionValue& ActionValue);
	
	/**
	* Applies input from MovementHorizontalAction to the Character.
	* @param ActionValue The Input Action's value.
	*/
	void MovementHorizontalActionHandler(const FInputActionValue& ActionValue);

	/**
	 * Applies input from MovementJumpAction to the Character.
	 */
	void MovementJumpActionHandler();

	/**
	* Applies input from MovementCrouchAction to the Character.
	* @param ActionValue The Input Action's value.
	*/
	void MovementCrouchActionHandler(const FInputActionValue& ActionValue);
	
	/**
	* Applies input from ViewVerticalAction to the Character.
	* @param ActionValue The Input Action's value.
	*/
	void ViewVerticalActionHandler(const FInputActionValue& ActionValue);

	/**
	* Applies input from ViewHorizontalAction to the Character.
	* @param ActionValue The Input Action's value.
	*/
	void ViewHorizontalActionHandler(const FInputActionValue& ActionValue);

	/**
	* Applies input from MouseWheelAction to the Character.
	* @param ActionValue The Input Action's value.
	*/
	void MouseWheelActionHandler(const FInputActionValue& ActionValue);

	/**
	* Applies input from LeanAction to the Character.
	* @param ActionValue The Input Action's value.
	*/
	void LeanActionHandler(const FInputActionValue& ActionValue);

	virtual void PawnClientRestart() override;
	
	FVector2D GetMouseInput() const
	{
		return MouseInput;
	}

	int32 GetLeanDirection() const
	{
		return LeanDirection;
	}

	FVector2D GetMovementInput() const
	{
		return MovementInput;
	}

	void SetCameraAnimRotation(FRotator NewRotation)
	{
		CameraAnimRotation = NewRotation;
	}

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentHealth() const
	{
		return CurrentHealth;
	}
};
