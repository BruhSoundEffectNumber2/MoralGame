// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Weapon.h"
#include "WeaponState.h"
#include "WeaponSwitcher.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MORALSHOOTER_API UWeaponSwitcher : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponSwitcher();

	/** The component that shows the weapon. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UChildActorComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AWeapon* WeaponActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AWeapon> StartingWeapon;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** The weapon we are currently holding. */
	UPROPERTY()
	int32 CurrentWeapon = 0;

	UPROPERTY()
	TArray<UWeaponState*> WeaponStates;

public:	
	// Called every frame
	virtual void TickComponent
	(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	void ChangeWeaponActor();

	/** Input for moving to the next weapon. */
	void InputNextWeapon();

	/** Input for moving to the previous weapon. */
	void InputPreviousWeapon();

	/** Input for moving to a specific weapon. */
	void InputSetWeapon(const FInputActionValue& ActionValue);

	/** For the weapons primary action (ie. firing a bullet on a trigger press) */
	virtual void InputPrimaryAction(const FInputActionValue& ActionValue);

	/** For the weapons secondary action (ie. aiming down the sight of a sniper rifle) */
	virtual void InputSecondaryAction(const FInputActionValue& ActionValue);

	/** For the reloading of weapons. */
	virtual void InputReload(const FInputActionValue& ActionValue);
};
