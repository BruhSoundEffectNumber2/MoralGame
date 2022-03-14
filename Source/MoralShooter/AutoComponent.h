// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunComponent.h"
#include "AutoComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MORALSHOOTER_API UAutoComponent : public UGunComponent
{
	GENERATED_BODY()

public:
	UAutoComponent();

protected:
	FTimerHandle TimerHandle;

	bool bWantsFire = false;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void Fire(bool Start) override;

	void ExecFire();
	
	UFUNCTION()
	void FireTimerComplete() const;
};
