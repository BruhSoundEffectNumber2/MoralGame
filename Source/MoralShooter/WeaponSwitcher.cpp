// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSwitcher.h"
#include "Gun.h"
#include "GunState.h"
#include "MoralCharacter.h"

// Sets default values for this component's properties
UWeaponSwitcher::UWeaponSwitcher()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UWeaponSwitcher::BeginPlay()
{
	Super::BeginPlay();

	WeaponStates.Add(NewObject<UGunState>());
	WeaponStates[0]->Class = StartingWeapon;
	WeaponStates.Add(NewObject<UGunState>());
	WeaponStates[1]->Class = StartingWeapon;

	// Find the child actor component
	if (UChildActorComponent* Child = Cast<UChildActorComponent>(GetChildComponent(0)))
	{
		WeaponComponent = Child;

		// Get the actor created by the component
		if (AWeapon* ChildActor = Cast<AWeapon>(WeaponComponent->GetChildActor()))
		{
			WeaponActor = ChildActor;
			WeaponActor->CharacterOwner = Cast<AMoralCharacter>(GetOwner());
			WeaponActor->Init(WeaponStates[0], !WeaponStates[0]->bInitialized);
			WeaponActor->Draw();
		}
	}
}

void UWeaponSwitcher::ChangeWeaponActor()
{
	WeaponComponent->DestroyChildActor();
	WeaponComponent->SetChildActorClass(WeaponStates[CurrentWeapon]->Class);
	WeaponComponent->CreateChildActor();

	// Get the actor created by the component
	if (AWeapon* ChildActor = Cast<AWeapon>(WeaponComponent->GetChildActor()))
	{
		WeaponActor = ChildActor;
		WeaponActor->CharacterOwner = Cast<AMoralCharacter>(GetOwner());
		WeaponActor->Init(WeaponStates[CurrentWeapon], !WeaponStates[CurrentWeapon]->bInitialized);
		WeaponActor->Draw();
	}
}

// Called every frame
void UWeaponSwitcher::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponSwitcher::InputNextWeapon()
{
	// Only switch if nothing is going on
	for (const UWeaponState* State : WeaponStates)
	{
		if (State->WeaponAction != EWeaponAction::Idle)
		{
			return;
		}
	}
	
	CurrentWeapon++;

	if (CurrentWeapon == WeaponStates.Num())
	{
		CurrentWeapon = 0;
	}

	WeaponActor->Holster();
}

void UWeaponSwitcher::InputPreviousWeapon()
{
	// Only switch if nothing is going on
	for (const UWeaponState* State : WeaponStates)
	{
		if (State->WeaponAction != EWeaponAction::Idle)
		{
			return;
		}
	}
	
	CurrentWeapon--;

	if (CurrentWeapon == -1)
	{
		CurrentWeapon = WeaponStates.Num() - 1;
	}
	
	WeaponActor->Holster();
}

void UWeaponSwitcher::InputSetWeapon(const FInputActionValue& ActionValue)
{
	
}

void UWeaponSwitcher::InputPrimaryAction(const FInputActionValue& ActionValue)
{
	if (WeaponComponent && WeaponActor)
	{
		WeaponActor->InputPrimaryAction(ActionValue);
	}
}

void UWeaponSwitcher::InputSecondaryAction(const FInputActionValue& ActionValue)
{
	if (WeaponComponent && WeaponActor)
	{
		WeaponActor->InputSecondaryAction(ActionValue);
	}
}

void UWeaponSwitcher::InputReload(const FInputActionValue& ActionValue)
{
	if (WeaponComponent && WeaponActor)
	{
		Cast<AGun>(WeaponActor)->InputReloadAction(ActionValue);
	}
}