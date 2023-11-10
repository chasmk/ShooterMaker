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
	
	
	/**
	 * @brief 每帧调用， 更新角色的运动状态变量
	 * @param DeltaSeconds 
	 */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	virtual void NativeInitializeAnimation() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Moement, meta = (AllowPrivateAccess = "true"))
	class ACharacter* ShooterCharacter;

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

public:
	
};
