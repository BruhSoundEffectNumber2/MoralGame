// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create bullet mesh
	BulletMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = BulletMeshComponent;

	// Create projectile movement
	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileComponent->bAutoActivate = false;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DistanceTravelled = FVector::Distance(StartLocation, GetActorLocation());

	if (DistanceTravelled >= Range)
	{
		Destroy();
	}
}

void ABullet::Init(
	const float GunDamageDropOffFactor,
	const float GunTorsoDamage,
	const int32 GunRange,
	AController* GunDamageInstigator)
{
	// Set values
	this->DamageDropOffFactor = GunDamageDropOffFactor;
	this->TorsoDamage = GunTorsoDamage;
	this->Range = GunRange;
	this->DamageInstigator = GunDamageInstigator;

	StartLocation = GetActorLocation();

	// Allow the projectile to start moving
	ProjectileComponent->SetVelocityInLocalSpace(FVector::ForwardVector * InitSpeed);
	ProjectileComponent->Activate();

	// Tell the bullet to ignore the instigator's Pawn
	BulletMeshComponent->IgnoreActorWhenMoving(GunDamageInstigator->GetPawn(), true);

	if (!ProjectileComponent->OnProjectileStop.Contains(this, FName("ProjectileStopped")))
	{
		ProjectileComponent->OnProjectileStop.AddDynamic(this, &ABullet::ProjectileStopped);
	}
}

void ABullet::ProjectileStopped(const FHitResult& Result)
{
	if (Result.GetActor())
	{
		// TODO: Damage based on what part of the body we hit
		const float DistanceFactor = DistanceTravelled / Range;
		const float DamageMultiplier = (-DamageDropOffFactor * (FMath::Pow(DistanceFactor, 10))) + 1.f;
		const FDamageEvent DamageEvent;
		
		Result.GetActor()->TakeDamage(TorsoDamage * DamageMultiplier, DamageEvent, DamageInstigator, this);
	}

	Destroy();
}
