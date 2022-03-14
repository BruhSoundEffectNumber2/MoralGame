// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Bullet.generated.h"

UCLASS()
class MORALSHOOTER_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BulletMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* ProjectileComponent;

	/** The initial speed of the bullet. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitSpeed = 4500.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float DamageDropOffFactor;

	float TorsoDamage;
	
	int32 Range;

	UPROPERTY()
	AController* DamageInstigator;

	FVector StartLocation;
	float DistanceTravelled;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	/** Sets the properties of the bullet from the weapon that fired it. */
	void Init(
		const float GunDamageDropOffFactor,
		const float GunTorsoDamage,
		const int32 GunRange,
		AController* GunDamageInstigator);

	UFUNCTION()
	void ProjectileStopped(const FHitResult& Result);
};
