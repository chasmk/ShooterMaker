// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

UCLASS()
class SHOOTERMAKER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AShooterPlayerController();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=HUD, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> HUDOverlayClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=HUD, meta = (AllowPrivateAccess = "true"))
	UUserWidget* HUDOverlay;

public:
};
