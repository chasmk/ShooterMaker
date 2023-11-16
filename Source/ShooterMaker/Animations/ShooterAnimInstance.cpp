// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "../Characters/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if(ShooterCharacter)
	{
		//更新速度
		FVector Velocity = ShooterCharacter->GetCharacterMovement()->Velocity;
		Velocity.Z = 0.f;
		Speed = Velocity.Size();
		
		//更新是否在空中
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		//更新是否在移动
		bIsAccelerating =
			ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

		//获取角色和相机的角度offset
		FRotator AimRotaion = ShooterCharacter->GetBaseAimRotation();//跟随相机旋转改变值
		FRotator MovementRotation = ShooterCharacter->GetVelocity().Rotation();//跟随输入方向改变值
			//UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity())两个效果好像一样
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotaion).Yaw;

		if (bIsAccelerating)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		
		// GEngine->AddOnScreenDebugMessage(
		// 		4,
		// 		0.f,
		// 		FColor::Orange,
		// 		FString::Printf(TEXT("OffsetYaw %f"), MovementOffsetYaw));

		bAiming = ShooterCharacter->GetAiming();
	}
}


void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	
}
