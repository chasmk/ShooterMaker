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
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;
	void DrawCrossHair();
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Initialization, meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	UTexture* CrossHairTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	UTexture* CrossHairTopTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	UTexture* CrossHairBottomTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	UTexture* CrossHairLeftTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	UTexture* CrossHairRightTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	UTexture* CrossHairDotTexture;
	

	
};
