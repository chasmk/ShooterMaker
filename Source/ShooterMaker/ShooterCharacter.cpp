// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "DrawDebugHelpers.h"
#include "Item.h"
#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/ShooterCameraComponent.h"
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
	bFiringBullet(false)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//以下初始化Actor Component
	
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

	//生成默认武器并接在手上
	EquipWeapon(SpawnDefaultWeapon());
	
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
	if (EquippedWeapon)
	{
		GEngine->AddOnScreenDebugMessage(
		1,
		0.f,
		FColor::Blue,
		FString::Printf(TEXT("WeaponState: %s"), *UEnum::GetValueAsString(EquippedWeapon->GetCurrentState())));
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

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{//生成默认的武器并接在右手上
		AWeapon* DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
		return DefaultWeapon;
	}
	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RHSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
			WeaponToEquip->SetToEquippedMode();
		}
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::Eis_Equipped);
	}
	
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
		EquippedWeapon->SetItemState(EItemState::Eis_Falling);
		EquippedWeapon->ThrowWeapon();
		EquippedWeapon = nullptr;
	}
}

void AShooterCharacter::SwapWeapon(AWeapon* NewWeapon)
{
	DropWeapon();
	EquipWeapon(NewWeapon);
}

void AShooterCharacter::SelectButtonPressed()
{
	//SwapWeapon(Cast<AWeapon>(CurrentHitItem));
	if (CurrentHitItem)
	{  
		CurrentHitItem->StartItemCurve(this);
	}
	else
	{
		DropWeapon();
	}
}

void AShooterCharacter::SelectButtonReleased()
{
}

void AShooterCharacter::FireWeapon()
{
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			3,
			0.15f,
			FColor::Red,
			FString("Fire111"),
			false
		);
	}*/

	if (EquippedWeapon == nullptr)
	{
		return;
	}
	
	//开枪声音
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), FireSound);
	}

	//播放开枪动画
	if (FireMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(FireMontage);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("StartFire"));
	}

	//获取枪口socket，接下来做烟雾trace和Hit特效
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		//生成枪口闪光
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		//初始化子弹轨迹首尾位置
		FVector Start = SocketTransform.GetLocation();
		FVector End;

		FHitResult HitResult;
		//根据trace情况获取轨迹位置
		if (GetCrosshairTraceResult(HitResult, End, 50'000.f) == true)
		{
			//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 10.f, 0, 5.f);

			//第二次trace
			bool bIsHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start,
				End,
				ECC_Visibility
			);

			if (bIsHit)
			{//第二次更新命中位置
				End = HitResult.Location;
			}
			
			//生成命中物体时的特效
			if (ImpactParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, End);
			}

			//生成子弹轨迹烟雾
			if (BeamParticle)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(), BeamParticle, Start);
				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), End);
				}
			}
		}
	}

	//用于准心放缩
	StartCrosshairBulletFire();
}

void AShooterCharacter::FireCallBack()
{
	FireWeapon();
}

void AShooterCharacter::FireWeaponPressed()
{
	FireWeapon();
	float FireInterval = 0.125f;
	GetWorldTimerManager().SetTimer(FireTimer, this, &AShooterCharacter::FireCallBack, FireInterval, true,
	                                FireInterval);
}

void AShooterCharacter::FireWeaponReleased()
{
	GetWorldTimerManager().ClearTimer(FireTimer);
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
	FVector tmp;//充数用
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
			if(!bIsTraced && TraceItems.Num() > 1)
			{//第一次追踪该item，其它item都设置为不可见
				for(AItem* i : TraceItems)
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
		SwapWeapon(Weapon);
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

}
