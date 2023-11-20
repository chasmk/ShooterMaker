// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "../Characters/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UShooterAnimInstance::UShooterAnimInstance() :
	Speed(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	MovementOffsetYaw(false),
	LastMovementOffsetYaw(false),
	bAiming(false),
	CharacterYaw(0.f),
	LastCharacterYaw(0.f),
	RootYawOffset(0.f),
	CurveYaw(-90.f),
	LeanRotation(FRotator(0.f)),
	LastLeanRotation(FRotator(0.f)),
	LeanYawOffset(0.f)
{
}

void UShooterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter)
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
		FRotator AimRotaion = ShooterCharacter->GetBaseAimRotation(); //跟随相机旋转改变值
		FRotator MovementRotation = ShooterCharacter->GetVelocity().Rotation(); //跟随输入方向改变值
		//UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity())两个效果好像一样
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotaion).Yaw;

		//更新最新的移动offset，用于jog stop
		if (bIsAccelerating)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		//更新原地旋转变量
		TurnInPlace();
		//更新lean变量
		Lean(DeltaSeconds);

		//更新是否在瞄准
		bAiming = ShooterCharacter->GetAiming();
	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter && Speed <= 0)
	{
		AOPitch = ShooterCharacter->GetBaseAimRotation().Pitch;

		if (GetCurveValue(FName(TEXT("Turning"))) == 1.f)
		{
			//此次正在播放turning动画
			LastCurveYaw = CurveYaw;
			CurveYaw = GetCurveValue(FName(TEXT("DistanceCurve")));
			const float CurveYawOffset = CurveYaw - LastCurveYaw;
			if (RootYawOffset > 0)
			{
				//向左转，root yaw需要减小
				RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - CurveYawOffset);
			}
			else
			{
				//向右转
				RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset + CurveYawOffset);
			}
			//补偿多出来的yaw
			const float ABSRootYawOffset = FMath::Abs(RootYawOffset);
			if (ABSRootYawOffset > 90.f)
			{
				const float YawExcess = ABSRootYawOffset - 90.f;
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
		else
		{
			CurveYaw = -90.f;
		}

		//正常更新yaw
		LastCharacterYaw = CharacterYaw;
		CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		const float YawOffset = CharacterYaw - LastCharacterYaw;
		//RootYawOffset clamp到[-180, 180]
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - YawOffset);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				5,
				-1,
				FColor::Purple,
				FString::Printf(TEXT("RootYawOffset: %f"), RootYawOffset));
		}
	}
	else
	{
		//再运动时，也要实时更新这些值
		CurveYaw = -90.f;
		RootYawOffset = 0.f;
		CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
	}
}

void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (ShooterCharacter == nullptr) return;

	LastLeanRotation = LeanRotation;
	LeanRotation = ShooterCharacter->GetActorRotation();
	//避免-180与180之间过渡时角色抽搐
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(LeanRotation, LastLeanRotation);

	//转动越快，该值越大，即lean的角度就越大
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(LeanYawOffset, Target, DeltaTime, 6.f);

	LeanYawOffset = FMath::Clamp(Interp, -90.f, 90.f);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			6,
			-1,
			FColor::White,
			FString::Printf(TEXT("LeanYawOffset: %f"), LeanYawOffset));
	}
}
