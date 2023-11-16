// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

//战斗状态enum
UENUM(BlueprintType)
enum class ECombatState : uint8
{
	Ecs_Unoccupied UMETA(DisplayName = "Unoccupied"),
	Ecs_Firing UMETA(DisplayName = "Firing"),
	Ecs_Reloading UMETA(DisplayName = "Reloading"),

	Ecs_MAX UMETA(DisplayName = "DefaultMax"),
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTERMAKER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
friend class AShooterCharacter;
	
public:

	
protected:
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

	
	class AWeapon* SpawnDefaultWeapon();
	void EquipWeapon(AWeapon* WeaponToEquip);
	void DropWeapon();// 把手里武器扔地上
	void SwapWeapon(AWeapon* NewWeapon);//面前有武器时与手里武器交换
	bool WeaponHasAmmo();//看当前持有的武器有没有子弹可用
	void FireWeapon(); //按下鼠标左键时触发
	void ReloadWeapon();
	
	


private:
	class AShooterCharacter* ShooterCharacter;
	
	ECombatState CurrentCombatState;

	/**
	* 开枪相关变量
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon; //当前装备的武器

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass; //蓝图中默认的设置

	
	
public:
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE ECombatState GetCurrentCombatState() const { return CurrentCombatState; }
	FORCEINLINE void SetCurrentCombatState(const ECombatState NewState) { CurrentCombatState = NewState; }
};
