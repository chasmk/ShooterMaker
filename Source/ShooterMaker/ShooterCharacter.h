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

	void FireWeapon(); //按下鼠标左键时触发
	void FireCallBack();//FireTimer的call back
	void FireWeaponPressed(); //按下鼠标左键时触发
	void FireWeaponReleased();
	bool GetDesiredTraceLocation(const FVector& Start, FVector& End); //获取真实的trace轨迹

	// 瞄准时设置bAiming
	void AimingButtonPressed();
	void AimingButtonReleased();
	void UpdateFOV();

	void CalculateCrosshairSpread(float DeltaTime);//计算准心实时偏移量

	void StartCrosshairBulletFire();//开启射击时timer
	void FinishCrosshairBulletFire();//射击时timer的call back

private:
	/**
	 *  相机相关
	 */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom; //用于连接角色和相机的弹簧臂
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate; //单位是 度/秒
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate; //单位是 度/秒，
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"),
		meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float HipTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"),
		meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float HipLookUpRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"),
		meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float AimingTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"),
		meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float AimingLookUpRate;

	/**
	 * 瞄准前后相机的FOV设置
	 */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	float CameraDefaultFOV;
	float CameraZoomedFOV;
	float CameraCurrentFOV;
	FTimerHandle FOVTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ZoomTimerInterval;

	/**
	 * 十字准心相关
	 */

	//决定准心放大
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = true))
	float CrosshairSpreadMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = true))
	float CrosshairVelocityFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = true))
	float CrosshairInAirFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = true))
	float CrosshairAimFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = true))
	float CrosshairShootingFactor;

	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

	/**
	 * 开枪相关变量
	 */
	
	FTimerHandle FireTimer;
	
	
	//随机的子弹声
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	//枪口的flash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	//开枪动画蒙太奇
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* FireMontage;

	//命中时的particle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle;

	//子弹轨迹particle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticle;

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool GetAiming() const { return bAiming; }
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const { return CrosshairSpreadMultiplier; }
};
