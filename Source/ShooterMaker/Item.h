// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Eir_Damaged UMETA(DisplayName = "Damaged"),
	Eir_Common UMETA(DisplayName = "Common"),
	Eir_UnCommon UMETA(DisplayName = "UnCommon"),
	Eir_Rare UMETA(DisplayName = "Rare"),
	Eir_Legendary UMETA(DisplayName = "Legendary"),

	Eir_Max UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	//待拾取状态
	Eis_Pickup UMETA(DisplayName = "Pickup"),
	//拾取后在空中飞状态
	Eis_EquipInterping UMETA(DisplayName = "EquipInterping"),
	//在背包中状态
	Eis_PickedUp UMETA(DisplayName = "PickedUp"),
	//拿在手中的状态
	Eis_Equipped UMETA(DisplayName = "Equipped"),
	//扔出枪在空中状态
	Eis_Falling UMETA(DisplayName = "Falling"),

	Eis_Max UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class SHOOTERMAKER_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	//根据更新的状态设置item属性
	void SetItemProperties(EItemState State);

	//ItemInterpTimer的call back
	void FinishInterping();

	//tick调用，更新item的位置
	void ItemInterp(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = true))
	class AShooterCharacter* ShooterCharacter;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* ItemMesh; //武器的mesh

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = true))
	class UBoxComponent* CollisionBox; //用于检测碰撞显示UI

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = true))
	class UWidgetComponent* PickupWidget; //角色看向这里时弹出的widget

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = true))
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = true))
	int AmmoCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = true))
	EItemRarity StarLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = true))
	EItemState ItemState; //item当前所处状态
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = true))
	class UCurveFloat* ItemZCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = true))
	FVector ItemInterpStartLocation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = true))
	FVector CameraTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = true))
	bool bInterping;

	FTimerHandle ItemInterpTimer;//武器在空中飞的计时器

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = true))
	float ZCurveTime;

	float InterpInitialYawOffset;//武器在空中时和相机的相对旋转偏移

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = true))
	UCurveFloat* ItemScaleCurve;//武器在空中时的大小放缩曲线
	
public:
	void SetToEquippedMode() const;
	void SetPickupWidgetVisibility(bool bVisible) const;
	void SetItemState(EItemState InState);
	FORCEINLINE EItemState GetCurrentState() const { return ItemState; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }

	//由角色类调用
	void StartItemCurve(AShooterCharacter* Character);
};
