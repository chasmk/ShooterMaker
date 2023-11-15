// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCameraComponent.h"

#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UShooterCameraComponent::UShooterCameraComponent() :
InterpFwdDistance(200.f),
InterpRtDistance(70.f),
InterpUpDistance(-60.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UShooterCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UShooterCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FVector UShooterCameraComponent::GetCameraInterpLocation()
{
	const FVector CameraWorldLocation = GetComponentLocation();
	const FVector CameraForward = GetForwardVector();
	//const FVector CameraUp = GetUpVector();
	const FVector CameraRight = GetRightVector();
	FVector DesiredLocation =
		CameraWorldLocation
	+ CameraForward * InterpFwdDistance
	+ FVector(0.f, 0.f, InterpUpDistance)
	+ CameraRight * InterpRtDistance;
	
	//DrawDebugPoint(GetWorld(), DesiredLocation, 5.f, FColor::Blue, false, 2.f);
	
	return DesiredLocation;
}

