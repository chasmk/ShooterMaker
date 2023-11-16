// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "ShooterMaker/Actors/Weapon.h"
#include "ShooterMaker/Characters/ShooterCharacter.h"
#include "Sound/SoundCue.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent():
	CurrentCombatState(ECombatState::Ecs_Unoccupied)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	ShooterCharacter = Cast<AShooterCharacter>(GetOwner());
}

AWeapon* UCombatComponent::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		//生成默认的武器并接在右手上
		AWeapon* DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
		return DefaultWeapon;
	}
	return nullptr;
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		const USkeletalMeshSocket* HandSocket = ShooterCharacter->GetMesh()->GetSocketByName(FName("RHSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToEquip, ShooterCharacter->GetMesh());
			WeaponToEquip->SetToEquippedMode();
		}
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::Eis_Equipped);
		ShooterCharacter->UpdateAmmoMap(EquippedWeapon->GetAmmoType(), 0);
	}
}

void UCombatComponent::DropWeapon()
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

void UCombatComponent::SwapWeapon(AWeapon* NewWeapon)
{
	DropWeapon();
	EquipWeapon(NewWeapon);
}

bool UCombatComponent::WeaponHasAmmo()
{
	if (!EquippedWeapon) return false;

	return EquippedWeapon->GetAmmo() == 0 ? false : true;
}

void UCombatComponent::FireWeapon()
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

	if (EquippedWeapon == nullptr || EquippedWeapon->GetAmmo() <=0 || GetCurrentCombatState() == ECombatState::Ecs_Reloading)
	{
		return;
	}

	//消耗子弹 1 
	if(EquippedWeapon->DecrementAmmo() <= 0)
	{
		ShooterCharacter->ReloadWeaponPressed();
		return;
	}

	//开枪声音
	if (EquippedWeapon->FireSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EquippedWeapon->FireSound);
	}

	//播放开枪动画
	if (EquippedWeapon->FireMontage)
	{
		ShooterCharacter->GetMesh()->GetAnimInstance()->Montage_Play(EquippedWeapon->FireMontage);
		ShooterCharacter->GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("StartFire"));
	}

	//获取枪口socket，接下来做烟雾trace和Hit特效
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetBarrelSocket();
	if (BarrelSocket)
	{
		//生成枪口闪光
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());
		if (EquippedWeapon->MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquippedWeapon->MuzzleFlash, SocketTransform);
		}

		//初始化子弹轨迹首尾位置
		FVector Start = SocketTransform.GetLocation();
		FVector End;

		FHitResult HitResult;
		//根据trace情况获取轨迹位置
		if (ShooterCharacter->GetCrosshairTraceResult(HitResult, End, 50'000.f) == true)
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
			{
				//第二次更新命中位置
				End = HitResult.Location;
			}

			//生成命中物体时的特效
			if (EquippedWeapon->ImpactParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquippedWeapon->ImpactParticle, End);
			}

			//生成子弹轨迹烟雾
			if (EquippedWeapon->BeamParticle)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(), EquippedWeapon->BeamParticle, Start);
				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), End);
				}
			}
		}
	}

	//用于准心放缩
	ShooterCharacter->StartCrosshairBulletFire();
}

void UCombatComponent::ReloadWeapon()
{
	//换弹声音
	if (EquippedWeapon->ReloadSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EquippedWeapon->ReloadSound);
	}

	//播放换弹动画
	if (EquippedWeapon->ReloadMontage)
	{
		const float PlayDuration = ShooterCharacter->GetMesh()->GetAnimInstance()->
		                                       Montage_Play(EquippedWeapon->ReloadMontage);
		ShooterCharacter->GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("ReloadSMG"));
		//ShooterCharacter->StartReloadTimer(PlayDuration);
	}
	
	
}



// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
