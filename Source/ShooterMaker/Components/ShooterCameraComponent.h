// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "ShooterCameraComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTERMAKER_API UShooterCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UShooterCameraComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

protected:

private:
	//以下是枪在空中飞的相对位置
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Interplate, meta = (AllowPrivateAccess = true))
	float InterpFwdDistance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Interplate, meta = (AllowPrivateAccess = true))
	float InterpRtDistance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Interplate, meta = (AllowPrivateAccess = true))
	float InterpUpDistance;

	
public:
	FVector GetCameraInterpLocation();
};
