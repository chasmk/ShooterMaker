// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class SHOOTERMAKER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/**
	 * @brief 构造函数用于初始化各种变量
	 */
	AShooterCharacter();
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override; // Called when the game starts or when spawned

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate); //Rate 介于0-1之间, 1代表100%是我们期望速率
	void LookUpAtRate(float Rate); //Rate 介于0-1之间

	void FireWeapon();//按下鼠标左键时触发
	bool GetDesiredTraceLocation(const FVector& Start, FVector& End);//获取真实的trace轨迹

private:
	/**
	 * @brief 用于连接角色和相机的弹簧臂
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate; //单位是 度/秒
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate; //单位是 度/秒，

	/** 随机的子弹声 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	/** 枪口的flash */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;
	
	/** 开枪动画蒙太奇 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* FireMontage;

	/**命中时的particle*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle;

	/**子弹轨迹particle*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticle;
	
public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
