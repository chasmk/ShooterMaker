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

private:
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


public:
	void SetToEquippedMode() const;
	void SetPickupWidgetVisibility(bool bVisible) const;
	void SetItemState(EItemState InState);
	FORCEINLINE EItemState GetCurrentState() const { return ItemState; }
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
};
