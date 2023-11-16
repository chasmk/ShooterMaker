// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "../Enums/AmmoType.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Ewt_SubMachineGun UMETA(DisplayName = "SubmachineGun"),
	Ewt_AssaultRifle UMETA(DisplayName = "AssaultRifle"),

	Ewt_Max UMETA(DisplayName = "DefaultMax"),
};

UCLASS()
class SHOOTERMAKER_API AWeapon : public AItem
{
	GENERATED_BODY()
friend class UCombatComponent;
	
public:
	AWeapon();
	virtual void Tick(float DeltaSeconds) override;
	void ThrowWeapon();

protected:
	void StopFalling();

private:
	/** 扔枪相关*/

	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;

	/** 一些武器属性 */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "武器属性", meta = (AllowPrivateAccess = true))
	FName BarrelSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "武器属性", meta = (AllowPrivateAccess = true))
	int32 Ammo;//当前子弹数

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "武器属性", meta = (AllowPrivateAccess = true))
	int32 AmmoCapacity;//弹夹最大子弹数

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "武器属性", meta = (AllowPrivateAccess = true))
	FString WeaponName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "武器属性", meta = (AllowPrivateAccess = true))
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "武器属性", meta = (AllowPrivateAccess = true))
	EAmmoType AmmoType;

	//随机的子弹声
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="武器属性", meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	//枪口的flash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="武器属性", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	//开枪动画蒙太奇
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="武器属性", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* FireMontage;

	//命中时的particle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="武器属性", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle;

	//子弹轨迹particle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="武器属性", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticle;
	
	//换弹动画蒙太奇
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="武器属性", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* ReloadMontage;

	//换弹声
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="武器属性", meta = (AllowPrivateAccess = "true"))
	class USoundCue* ReloadSound;


public:
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMaxAmmo() const { return AmmoCapacity; }
	int32 DecrementAmmo(); //角色类开火时调用
	void IncrementAmmo(int32 DeltaValue); //角色类开火时调用
	class USkeletalMeshSocket const* GetBarrelSocket();
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
};
