// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunComponent.h"
#include "SelfLoadMagComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MORALSHOOTER_API USelfLoadMagComponent : public UGunComponent
{
	GENERATED_BODY()

public:
	USelfLoadMagComponent();

protected:
	FTimerHandle TimerHandle;

public:
	virtual void Reload() override;
	
	UFUNCTION()
	void ReloadTimerComplete() const;
};
