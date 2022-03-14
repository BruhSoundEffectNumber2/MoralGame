// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GunComponent.generated.h"

// Forward declarations
class AGun;

/* Defines what behavior does a GunComponent affect. */
UENUM(BlueprintType)
enum class EGunComponentBehavior : uint8
{
	Fire,
	Reload
};

/* Defines a behavior for a Gun to perform. */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MORALSHOOTER_API UGunComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGunComponent();

	EGunComponentBehavior Behavior;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	AGun* Owner;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Init(AGun* OwnerPointer);

	virtual void Fire(bool Start);

	virtual void Reload();
};
