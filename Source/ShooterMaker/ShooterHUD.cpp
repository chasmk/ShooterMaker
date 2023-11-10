// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterHUD.h"

#include "Engine/Canvas.h"

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();
	
	DrawCrossHair();
	
}

void AShooterHUD::DrawCrossHair()
{
	if (CrossHairTexture)
	{
		const FVector2D ScreenCenter = FVector2D(Canvas->SizeX/2, Canvas->SizeY/2);
		DrawTexture(CrossHairTexture,
			ScreenCenter.X - 32,
			ScreenCenter.Y - 60,
			64,
			64,
			0,
			0,
			1,
			1
			);
		
	}
	
}
