// Fill out your copyright notice in the Description page of Project Settings.


#include "GunComponent.h"

// Sets default values for this component's properties
UGunComponent::UGunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGunComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UGunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGunComponent::Fire(bool Start)
{
	UE_LOG(LogTemp, Display, TEXT("GunComponent Fire"))
}

void UGunComponent::Reload()
{
	UE_LOG(LogTemp, Display, TEXT("GunComponent Reload"))
}

void UGunComponent::Init(AGun* OwnerPointer)
{
	Owner = OwnerPointer;
}
