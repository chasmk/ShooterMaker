// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterHUD.h"

#include "ShooterCharacter.h"
#include "Engine/Canvas.h"

void AShooterHUD::BeginPlay()
{
	Super::BeginPlay();

	ShooterCharacter = Cast<AShooterCharacter>(GetOwningPawn());
}

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();
	
	DrawCrossHair();
	
}

void AShooterHUD::DrawCrossHair()
{
	const FVector2D ScreenCenter = FVector2D(Canvas->SizeX/2, Canvas->SizeY/2);
	const float CrosshairWidth = ScreenCenter.X * 0.05;//计算准心的大小
	const float CrosshairX = ScreenCenter.X - CrosshairWidth / 2;
	const float CrosshairY = ScreenCenter.Y - CrosshairWidth / 2; // - 28.f;
	
	/*GEngine->AddOnScreenDebugMessage(
			1,
			0.f,
			FColor::Blue,
			FString::Printf(TEXT("CrosshairWidth %f"), CrosshairWidth));*/ 
	
	//根据角色类更新的偏移量 画四条准心
	if (CrossHairTopTexture && CrossHairBottomTexture && CrossHairLeftTexture && CrossHairRightTexture)
	{
		//获取实时偏移量，乘一个数使其放大
		const float SpreadMultiplier = 6.f * ShooterCharacter->GetCrosshairSpreadMultiplier();
		//Top
		DrawTexture(CrossHairTopTexture,
			CrosshairX,
			CrosshairY - SpreadMultiplier,
			CrosshairWidth,
			CrosshairWidth,
			0,
			0,
			1,
			1
			);
		//Bottom
		DrawTexture(CrossHairBottomTexture,
			CrosshairX,
			CrosshairY + SpreadMultiplier,
			CrosshairWidth,
			CrosshairWidth,
			0,
			0,
			1,
			1
			);
		//Left
		DrawTexture(CrossHairLeftTexture,
			CrosshairX - SpreadMultiplier,
			CrosshairY,
			CrosshairWidth,
			CrosshairWidth,
			0,
			0,
			1,
			1
			);
		//right
		DrawTexture(CrossHairRightTexture,
			CrosshairX + SpreadMultiplier,
			CrosshairY,
			CrosshairWidth,
			CrosshairWidth,
			0,
			0,
			1,
			1
			);
	}
	

	/*画整个准心
	if (CrossHairTexture)
	{
		DrawTexture(CrossHairTexture,
			ScreenCenter.X - CrosshairWidth / 2,
			ScreenCenter.Y - CrosshairWidth / 2 - 28.f,
			CrosshairWidth,
			CrosshairWidth,
			0,
			0,
			1,
			1
			);
	}*/

	//画准心中间的点
	// if (CrossHairDotTexture)
	// {
	// 	const float DotSize = CrosshairWidth;
	// 	DrawTexture(CrossHairDotTexture,
	// 		ScreenCenter.X - CrosshairWidth / 2,
	// 		ScreenCenter.Y - CrosshairWidth / 2 - 28.f,
	// 		DotSize,
	// 		DotSize,
	// 		0,
	// 		0,
	// 		1,
	// 		1
	// 		);
	// }
	
}
