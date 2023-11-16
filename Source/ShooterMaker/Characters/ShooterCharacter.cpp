// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "DrawDebugHelpers.h"
#include "../Actors/Item.h"
#include "../Actors/Weapon.h"
#include "Components/BoxComponent.h"
#include "../Components/CombatComponent.h"
#include "../Components/ShooterCameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"


AShooterCharacter::AShooterCharacter() :
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	// 正常/瞄准时的 Turn Rates
	HipTurnRate(45.f),
	HipLookUpRate(45.f),
	AimingTurnRate(20.f),
	AimingLookUpRate(20.f),
	bAiming(false),
	//相机FOV
	CameraDefaultFOV(0.f), // 在begin play中再初始化
	CameraZoomedFOV(40.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(18),
	ZoomTimerInterval(0.01),
	//准心放缩factor
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),
	//准心缩放 shoot
	ShootTimeDuration(0.05f),
	bFiringBullet(false),
	//初始子弹数
	Starting9mmAmmo(60),
	StartingARAmmo(90)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//以下初始化Actor Component
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	//弹簧臂初始化
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f; //长度
	CameraBoom->bUsePawnControlRotation = true; //弹簧臂跟随controller旋转
	CameraBoom->SocketOffset = FVector(0.f, 120.f, 70.f);

	//相机初始化
	FollowCamera = CreateDefaultSubobject<UShooterCameraComponent>(TEXT("FollowCamera222"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; //相机不跟随controller旋转

	//角色不跟随controller(鼠标)旋转，controller只影响camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true; //true: 左右旋转跟随鼠标转
	bUseControllerRotationRoll = false;


	GetCharacterMovement()->bOrientRotationToMovement = false; //角色不跟随输入的方向旋转
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f; //[0, 1]在空中时输入的影响
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetFollowCamera())
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	BaseTurnRate = HipTurnRate;
	BaseLookUpRate = HipLookUpRate;


	InitializeAmmoMap(); //初始化子弹数

	//生成默认武器并接在手上
	CombatComponent->EquipWeapon(CombatComponent->SpawnDefaultWeapon());

	//设置trace Item的timer
	GetWorldTimerManager().SetTimer(
		ItemTraceTimer,
		this,
		&AShooterCharacter::TraceItem,
		0.1f,
		true);
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/* Yaw Offset调试
	GEngine->AddOnScreenDebugMessage(
		1,
		0.f,
		FColor::Blue,
		FString::Printf(TEXT("BaseAimRotation %f"), GetBaseAimRotation().Yaw));
	GEngine->AddOnScreenDebugMessage(
		2,
		0.f,
		FColor::Green,
		FString::Printf(TEXT("ControlRotation %f"), GetControlRotation().Yaw));
	GEngine->AddOnScreenDebugMessage(
		3,
		0.f,
		FColor::Red,
		FString::Printf(TEXT("VelocityRotation %f"), GetVelocity().Rotation().Yaw));
*/
	if (CombatComponent->GetEquippedWeapon())
	{
		GEngine->AddOnScreenDebugMessage(
			1,
			0.f,
			FColor::Blue,
			FString::Printf(
				TEXT("CombatState: %s"), *UEnum::GetValueAsString(CombatComponent->GetCurrentCombatState())));
	}
	CalculateCrosshairSpread(DeltaTime);
}

void AShooterCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.f)
	{
		const FRotator Rotation{Controller->GetControlRotation()};
		const FRotator YawRotation{0.f, Rotation.Yaw, 0.f};
		const FVector Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X)};
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.f)
	{
		const FRotator Rotation{Controller->GetControlRotation()};
		const FRotator YawRotation{0.f, Rotation.Yaw, 0.f};
		const FVector Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y)};
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	// deg/sec * sec/frame = deg/frame
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::Eat_9MM, 0);
	AmmoMap.Add(EAmmoType::Eat_AR, 0);

	UpdateAmmoMap(EAmmoType::Eat_9MM, Starting9mmAmmo);
	UpdateAmmoMap(EAmmoType::Eat_AR, StartingARAmmo);
}

void AShooterCharacter::SelectButtonPressed()
{
	//SwapWeapon(Cast<AWeapon>(CurrentHitItem));
	if (CurrentHitItem)
	{
		CurrentHitItem->StartItemCurve(this);

		if (CurrentHitItem->GetPickupSound())
		{//播放拾取声音
			UGameplayStatics::PlaySound2D(GetWorld(), CurrentHitItem->GetPickupSound());
		}
	}
	else
	{
		CombatComponent->DropWeapon();
	}
}

void AShooterCharacter::SelectButtonReleased()
{
}

AWeapon* AShooterCharacter::GetWeapon() const
{
	return CombatComponent->GetEquippedWeapon();
}

void AShooterCharacter::FireCallBack()
{
	CombatComponent->FireWeapon();
}

void AShooterCharacter::FireWeaponPressed()
{
	bFireBuutonPressed = true;
	CombatComponent->FireWeapon();
	float FireInterval = 0.125f;
	GetWorldTimerManager().SetTimer(FireTimer, this, &AShooterCharacter::FireCallBack, FireInterval, true,
	                                FireInterval);
	if (CombatComponent->GetCurrentCombatState() != ECombatState::Ecs_Reloading)
	{
		CombatComponent->SetCurrentCombatState(ECombatState::Ecs_Firing);
	}
}

void AShooterCharacter::FireWeaponReleased()
{
	bFireBuutonPressed = false;
	GetWorldTimerManager().ClearTimer(FireTimer);
	if (CombatComponent->GetCurrentCombatState() != ECombatState::Ecs_Reloading)
	{
		CombatComponent->SetCurrentCombatState(ECombatState::Ecs_Unoccupied);
	}
}

bool AShooterCharacter::CanReload()
{
	if (HaveCarringAmmo() && GetWeapon()->GetAmmo() < GetWeapon()->GetMaxAmmo() && CombatComponent->GetCurrentCombatState() != ECombatState::Ecs_Reloading)
	{
		return true;
	}
	return false;
}

void AShooterCharacter::StartReloadTimer(float Duration)
{
	GetWorldTimerManager().SetTimer(
		ReloadTimer,
		this,
		&AShooterCharacter::ReloadWeaponComplete,
		Duration);
}

void AShooterCharacter::ReloadWeaponComplete()
{
	CombatComponent->
		SetCurrentCombatState(bFireBuutonPressed ? ECombatState::Ecs_Firing : ECombatState::Ecs_Unoccupied);
	UpdateAmmo();
}

void AShooterCharacter::ReloadWeaponPressed()
{
	if (CanReload())
	{
		CombatComponent->SetCurrentCombatState(ECombatState::Ecs_Reloading);
		CombatComponent->ReloadWeapon();
	}
}

int32 AShooterCharacter::HaveCarringAmmo()
{
	if (AmmoMap.Contains(CombatComponent->EquippedWeapon->GetAmmoType()))
	{
		const int32 RestAmmo = AmmoMap[CombatComponent->EquippedWeapon->GetAmmoType()];
		return RestAmmo > 0;
	}
	return 0;
}

void AShooterCharacter::UpdateAmmoMap(EAmmoType AmmoType, int32 DeltaCount)
{
	AmmoMap[AmmoType] = ((AmmoMap[AmmoType] + DeltaCount) > 0) ? AmmoMap[AmmoType] + DeltaCount : 0;

	if (CombatComponent->EquippedWeapon && AmmoMap.Contains(CombatComponent->EquippedWeapon->GetAmmoType()))
	{
		CurrentCarryAmmo = AmmoMap[CombatComponent->EquippedWeapon->GetAmmoType()]; //每次都更新
	}
}

void AShooterCharacter::UpdateAmmo()
{
	if (HaveCarringAmmo())
	{
		//需要的换弹数
		int32 NeedAmmo = GetWeapon()->GetMaxAmmo() - GetWeapon()->GetAmmo();
		//可提供的换弹数
		int32 ProvideAmmo = AmmoMap[GetWeapon()->GetAmmoType()];
		//真实的换弹数
		int32 ActualAmmo = FMath::Clamp(NeedAmmo, 0, ProvideAmmo);

		//更新当前弹药和储备弹药
		UpdateAmmoMap(GetWeapon()->GetAmmoType(), -ActualAmmo);
		GetWeapon()->IncrementAmmo(ActualAmmo);
	}
}

void AShooterCharacter::GrabClip()
{
}

void AShooterCharacter::ReleaseClip()
{
}

bool AShooterCharacter::GetCrosshairTraceResult(FHitResult& OutHitResult, FVector& End, float TraceDistance)
{
	//获取屏幕上十字准心位置
	if (GEngine && GEngine->GameViewport)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);

		FVector2D CrosshairLocation = FVector2D(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		//CrosshairLocation.Y -= 28.f;

		//把十字准星从屏幕投影到3d空间(即相机的位置)
		FVector CrosshairWorldLocation;
		FVector CrosshairWorldDirection;
		bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
			UGameplayStatics::GetPlayerController(this, 0),
			CrosshairLocation,
			CrosshairWorldLocation,
			CrosshairWorldDirection
		);

		if (bScreenToWorld)
		{
			//trace轨迹end位置
			End = CrosshairWorldLocation + CrosshairWorldDirection * TraceDistance;

			//若十字准星发射的line能命中目标，则更新End的位置
			bool bIsCrosshairHit = GetWorld()->LineTraceSingleByChannel(
				OutHitResult,
				CrosshairWorldLocation,
				End,
				ECC_Visibility
			);

			if (bIsCrosshairHit)
			{
				//更新子弹轨迹end位置
				End = OutHitResult.Location;
			}
			return bIsCrosshairHit;
		}
		return bScreenToWorld;
	}
	else
	{
		return false;
	}
}

void AShooterCharacter::AimingButtonPressed()
{
	bAiming = true;

	BaseTurnRate = AimingTurnRate;
	BaseLookUpRate = AimingLookUpRate;

	GetWorldTimerManager().SetTimer(FOVTimer, this, &AShooterCharacter::UpdateFOV, ZoomTimerInterval, true, -1.f);
}

void AShooterCharacter::AimingButtonReleased()
{
	bAiming = false;

	BaseTurnRate = HipTurnRate;
	BaseLookUpRate = HipLookUpRate;

	GetWorldTimerManager().SetTimer(FOVTimer, this, &AShooterCharacter::UpdateFOV, ZoomTimerInterval, true, -1.f);
}

void AShooterCharacter::UpdateFOV()
{
	if (bAiming)
	{
		if (CameraCurrentFOV <= CameraZoomedFOV)
		{
			GetWorldTimerManager().ClearTimer(FOVTimer);
		}
		else
		{
			CameraCurrentFOV = FMath::FInterpTo(
				CameraCurrentFOV,
				CameraZoomedFOV,
				ZoomTimerInterval, //UGameplayStatics::GetWorldDeltaSeconds(GetWorld()),
				ZoomInterpSpeed
			);
			//CameraCurrentFOV -= ZoomInterpSpeed;
			GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
		}
	}
	else
	{
		if (CameraCurrentFOV >= CameraDefaultFOV)
		{
			GetWorldTimerManager().ClearTimer(FOVTimer);
		}
		else
		{
			CameraCurrentFOV = FMath::FInterpTo(
				CameraCurrentFOV,
				CameraDefaultFOV,
				ZoomTimerInterval,
				ZoomInterpSpeed
			);
			//CameraCurrentFOV += ZoomInterpSpeed;
			GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
		}
	}
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	//计算Velocity Factor
	FVector2D WalkSpeedRange = {0.f, GetCharacterMovement()->MaxWalkSpeed};
	FVector2D VelocityMulRange = {0.f, 1.f};
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped( //把当前速度map到[0,1]
		WalkSpeedRange,
		VelocityMulRange,
		GetVelocity().Size2D());

	//计算air factor
	if (GetCharacterMovement()->IsFalling())
	{
		float Delta = CrosshairInAirFactor + 0.01f;
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 1.f, DeltaTime, 2.5);
	}
	else
	{
		float Delta = CrosshairInAirFactor - 0.08f;
		CrosshairInAirFactor = FMath::Clamp(Delta, 0.f, 1.f);
	}

	//计算aim factor
	if (bAiming)
	{
		float Delta = CrosshairAimFactor - 0.01f;
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, -0.8f, DeltaTime, 10);
	}
	else
	{
		float Delta = CrosshairAimFactor + 0.08f;
		CrosshairAimFactor = FMath::Clamp(Delta, -1.f, 0.f);
	}

	//计算shoot factor
	if (bFiringBullet)
	{
		float Delta = CrosshairShootingFactor + 0.01f;
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.5f, DeltaTime, 50);
	}
	else
	{
		float Delta = CrosshairShootingFactor - 0.08f;
		CrosshairShootingFactor = FMath::Clamp(Delta, 0.f, 1.f);
	}

	//计算最终的Delta值，HUD类会读取
	CrosshairSpreadMultiplier = 0.5f
		+ CrosshairVelocityFactor
		+ CrosshairInAirFactor
		+ CrosshairAimFactor
		+ CrosshairShootingFactor;
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;
	GetWorldTimerManager().SetTimer(
		CrosshairShootTimer,
		this,
		&AShooterCharacter::FinishCrosshairBulletFire,
		ShootTimeDuration,
		false,
		0.f);
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

void AShooterCharacter::TraceItem()
{
	UKismetSystemLibrary::Delay(GetWorld(), 0.2f, FLatentActionInfo());
	FHitResult ItemTraceResult;
	CurrentHitItem = nullptr;
	FVector tmp; //充数用
	if (GetCrosshairTraceResult(ItemTraceResult, tmp, 900.f))
	{
		/*GEngine->AddOnScreenDebugMessage(
			0,
			0.f,
			FColor::Blue,
			FString::Printf(TEXT("111")));*/

		CurrentHitItem = Cast<AItem>(ItemTraceResult.GetActor());
		if (CurrentHitItem)
		{
			LastTraceItem = CurrentHitItem;
			CurrentHitItem->SetPickupWidgetVisibility(true);
			bool bIsTraced;
			TraceItems.Add(CurrentHitItem, &bIsTraced);
			if (!bIsTraced && TraceItems.Num() > 1)
			{
				//第一次追踪该item，其它item都设置为不可见
				for (AItem* i : TraceItems)
				{
					if (i != CurrentHitItem)
					{
						i->SetPickupWidgetVisibility(false);
						//TraceItems.Remove(i);
					}
				}
				TraceItems.Reset();
				TraceItems.Add(CurrentHitItem);
			}
		}
		else if (LastTraceItem)
		{
			LastTraceItem->SetPickupWidgetVisibility(false);
		}
	}
	else if (LastTraceItem)
	{
		LastTraceItem->SetPickupWidgetVisibility(false);
	}
}

void AShooterCharacter::GetPickupItem(AItem* Item)
{
	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		CombatComponent->SwapWeapon(Weapon);
		if (Weapon->GetEquipSound())
		{//播放装备声音
			UGameplayStatics::PlaySound2D(GetWorld(), Weapon->GetEquipSound());
		}
	}
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);

	//使用恒定速率
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUpAtRate);
	//使用鼠标速率
	//PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	//PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	// PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeaponPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireWeaponReleased);
	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);
	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this, &AShooterCharacter::SelectButtonReleased);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AShooterCharacter::ReloadWeaponPressed);
}
