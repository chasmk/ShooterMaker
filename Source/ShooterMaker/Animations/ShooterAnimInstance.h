// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMAKER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UShooterAnimInstance();

	/**
	 * @brief 每帧调用， 更新角色的运动状态变量
	 * @param DeltaSeconds 
	 */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void NativeInitializeAnimation() override;

protected:
	//更新原地旋转的变量值
	void TurnInPlace();
	//更新运动时Lean相关变量
	void Lean(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Moement, meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	//角色当前速度
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	//角色是否在空中
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	// 判断角色是否在移动
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	//Yaw Offset, 用于制作Strafe
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	//保存上一帧的Yaw Offset, 用于制作jog stop，在速度为0时停止更新，否则jog stop总是fwd的
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess = "true"))
	bool bAiming; //是否在瞄准

	/***
	 * 以下用于Turn In Place
	 */

	float CharacterYaw;
	float LastCharacterYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=TurnInPlace, meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;
	float CurveYaw;
	float LastCurveYaw;

	/**
	 * 以下用于 Lean
	 */

	FRotator LeanRotation;
	FRotator LastLeanRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Lean, meta = (AllowPrivateAccess = "true"))
	float LeanYawOffset;

	/**
	 * 以下用于Aim Offset
	 */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=TurnInPlace, meta = (AllowPrivateAccess = "true"))
	float AOPitch;

public:
};
