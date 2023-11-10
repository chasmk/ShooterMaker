// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMAKER_API AShooterHUD : public AHUD
{
	GENERATED_BODY()
public:

protected:
	virtual void DrawHUD() override;
	void DrawCrossHair();
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	UTexture* CrossHairTexture;

	
};
