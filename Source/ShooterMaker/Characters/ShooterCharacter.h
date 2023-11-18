// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Enums/AmmoType.h"
#include "ShooterCharacter.generated.h"


UCLASS()
class SHOOTERMAKER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	friend class UCombatComponent;

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
	void ToggleView();

	void InitializeAmmoMap();

	void SelectButtonPressed();
	void SelectButtonReleased();

	class AWeapon* GetWeapon() const;
	void FireCallBack();
	void FireWeaponPressed(); //按下鼠标左键时触发
	void FireWeaponReleased();

	void StartReloadTimer(float Duration);
	UFUNCTION(BlueprintCallable)
	void ReloadWeaponComplete();
	void ReloadWeaponPressed(); //按下R键时触发
	bool CanReload();//是否可执行换弹
	int32 HaveCarringAmmo();//有没有可换的弹药
	void UpdateAmmoMap(EAmmoType AmmoType, int32 DeltaCount);//更新携带的弹药
	void UpdateAmmo();//更新武器上的弹药
	UFUNCTION(BlueprintCallable)
	void GrabClip();//手抓住弹夹
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();//手松开弹夹


	// 瞄准时设置bAiming
	void AimingButtonPressed();
	void AimingButtonReleased();
	void UpdateFOV();
	void CalculateCrosshairSpread(float DeltaTime); //计算准心实时偏移量
	void StartCrosshairBulletFire(); //开启射击时timer
	void FinishCrosshairBulletFire(); //射击时timer的call back

	//从准心出发trace item
	void TraceItem();

	//获取准心的trace结果的trace轨迹
	bool GetCrosshairTraceResult(FHitResult& OutHitResult, FVector& End, float TraceDistance);

private:
	/** 
	 * Actor Components
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Actor Components", meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComponent;

	/**
	 *  相机相关
	 */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom; //用于连接角色和相机的弹簧臂
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta = (AllowPrivateAccess = "true"))
	class UShooterCameraComponent* FollowCamera;//第三人称相机
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta = (AllowPrivateAccess = "true"))
	class UShooterCameraComponent* FollowCameraFP;//第一人称相机
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bIsThirdView;//标记当前是第几人称视角

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

	FTimerHandle ItemTraceTimer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Trace, meta = (AllowPrivateAccess = true))
	class AItem* LastTraceItem;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Trace, meta = (AllowPrivateAccess = true))
	AItem* CurrentHitItem;
	TSet<AItem*> TraceItems;

	/**
	 * 开枪相关变量
	 */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta = (AllowPrivateAccess = "true"))
	bool bFireBuutonPressed;

	FTimerHandle FireTimer;
	FTimerHandle ReloadTimer;
	/**
	 * 子弹相关
	 */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap; //存放各种子弹的数量

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo; //9mm初始子弹数

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo; //ar初始子弹数

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 CurrentCarryAmmo;//当前武器的携带弹药数

	bool bMovingClip;//是否抓着弹夹

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UShooterCameraComponent* GetFollowCamera() const { return bIsThirdView ? FollowCamera : FollowCameraFP; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }

	FORCEINLINE bool GetAiming() const { return bAiming; }
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const { return CrosshairSpreadMultiplier; }

	void GetPickupItem(AItem* Item);
};
