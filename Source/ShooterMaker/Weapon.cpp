// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "DrawDebugHelpers.h"


AWeapon::AWeapon():
	ThrowWeaponTime(0.8f),
	bFalling(false)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//武器下落期间 让武器保持竖直
	if (GetCurrentState() == EItemState::Eis_Falling && bFalling)
	{
		FRotator MeshRotation = FRotator(0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f);
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	}
}

void AWeapon::ThrowWeapon()
{
	//先让武器竖直
	FRotator MeshRotation = FRotator(0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f);
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward = GetItemMesh()->GetForwardVector();
	const FVector MeshRight = GetItemMesh()->GetRightVector();//这里是武器正对方向
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);//扔出武器的方向，枪头向下转20度

	float RandomRotation = FMath::RandRange(10.f, 30.f);
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, GetActorUpVector());//绕z轴随机转个角度

	ImpulseDirection *= 20000.f;
	GetItemMesh()->AddImpulse(ImpulseDirection);//把武器发射出去
	
	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
}

void AWeapon::StopFalling()
{
	bFalling = false;
	SetItemState(EItemState::Eis_Pickup);
}
