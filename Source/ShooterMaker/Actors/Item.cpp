// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "../Characters/ShooterCharacter.h"
#include "TimerManager.h"
#include "../../../../Program Files/Epic Games/UE_4.27/Engine/Plugins/Marketplace/Developer/RiderLink/Source/RD/thirdparty/spdlog/include/spdlog/fmt/bundled/core.h"
#include "Components/BoxComponent.h"
#include "../Components/ShooterCameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"


// Sets default values
AItem::AItem() :
	ItemName(FString("DefaultName")),
	AmmoCount(0),
	StarLevel(EItemRarity::Eir_Common),
	ItemState(EItemState::Eis_Pickup),
	ZCurveTime(0.7f),
	ItemInterpStartLocation(FVector(0.f)),
	CameraTargetLocation(FVector(0.f)),
	bInterping(false),
	InterpInitialYawOffset(0.f)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(ItemMesh);
	PickupWidget->SetVisibility(false); //默认不可见
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	SetItemState(ItemState);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ItemInterp(DeltaTime);
}

void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::Eis_Pickup:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SetPickupWidgetVisibility(false);
		break;
	case EItemState::Eis_EquipInterping:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetPickupWidgetVisibility(false);
		break;
	case EItemState::Eis_PickedUp:
	case EItemState::Eis_Equipped:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetPickupWidgetVisibility(false);
		break;
	case EItemState::Eis_Falling:
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetPickupWidgetVisibility(false);
		break;
	default:
		break;
	}
}

void AItem::FinishInterping()
{
	bInterping = false;
	if (ShooterCharacter)
	{
		//计时器结束后通知角色交换枪
		ShooterCharacter->GetPickupItem(this);
	}
	SetActorScale3D(FVector(1.f));//武器恢复原来大小
}

void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping) return;

	if (ShooterCharacter && ItemZCurve && ItemScaleCurve)
	{
		//更新Location
		CameraTargetLocation = ShooterCharacter->GetFollowCamera()->GetCameraInterpLocation();
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
		FVector CurrentLocation = FMath::VInterpTo(GetActorLocation(), CameraTargetLocation, DeltaTime, 3.f);
		CurrentLocation.Z = ItemInterpStartLocation.Z + CurveValue * (CameraTargetLocation.Z - ItemInterpStartLocation.
			Z);
		SetActorLocation(CurrentLocation, false, nullptr, ETeleportType::None);

		//更新Rotation
		//FRotator CurrentCameraRotation = ShooterCharacter->GetFollowCamera()->GetComponentRotation();
		FRotator CurrentCameraRotation = ShooterCharacter->GetControlRotation();
		CurrentCameraRotation += FRotator(0.f, 180.f, 0.f);
		CurrentCameraRotation = FRotator(-CurrentCameraRotation.Pitch, CurrentCameraRotation.Yaw, CurrentCameraRotation.Roll);
		FRotator CurrentItemRotation = GetActorRotation();
		//CurrentItemRotation.Yaw = InterpInitialYawOffset + CurrentCameraRotation.Yaw;//保持武器相对旋转不变
		CurrentItemRotation = FMath::RInterpTo(CurrentItemRotation, CurrentCameraRotation, DeltaTime, 6.f);//旋转到侧面展示
		SetActorRotation(CurrentItemRotation);

		//更新Scale
		const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
		SetActorScale3D(FVector(ScaleCurveValue));
	}
}

void AItem::SetToEquippedMode() const
{
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	SetPickupWidgetVisibility(false);
}

void AItem::SetPickupWidgetVisibility(bool bVisible) const
{
	if (PickupWidget && (ItemState == EItemState::Eis_Pickup || ItemState == EItemState::Eis_EquipInterping))
	{
		PickupWidget->SetVisibility(bVisible);
	}
}

void AItem::SetItemState(EItemState InState)
{
	ItemState = InState;
	SetItemProperties(InState);
}

void AItem::StartItemCurve(AShooterCharacter* Character)
{
	ShooterCharacter = Character;
	ItemInterpStartLocation = GetActorLocation();
	CameraTargetLocation = ShooterCharacter->GetFollowCamera()->GetCameraInterpLocation();
	bInterping = true;
	SetItemState(EItemState::Eis_EquipInterping);

	GetWorldTimerManager().SetTimer(ItemInterpTimer, this, &AItem::FinishInterping, ZCurveTime);

	const float CameraYaw = ShooterCharacter->GetFollowCamera()->GetComponentRotation().Yaw;
	const float ItemYaw = GetActorRotation().Yaw;
	InterpInitialYawOffset = ItemYaw - CameraYaw;
}
