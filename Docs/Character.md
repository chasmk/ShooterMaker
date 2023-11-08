# Character

> 这个文档描述如何在C++中从最基础的ACharacter类建立功能完善的角色类，作为自己学习过程的记录。

## 0 准备工作

首先创建ACharacter的C++子类ShooterCharacter，并在编辑器中创建相应的蓝图类BP_ShooterCharcter。

打开蓝图可以发现角色类有一些默认组件：

- Capsule Componennt：胶囊体，代表角色在游戏中的碰撞，它作为Root所有组件只能接在它下面。
- Arrow，用来指示角色正面面向的方向
- Movement Component，管理角色运动

打开C++类有一些默认函数：

- AShooterCharacter()： 构造函数，用来初始化变量
- BeginPlay()：游戏开始或角色在关卡里生成时调用
- SetupPlayerInputComponent(): 用来绑定用户输入的函数，由PlayerController调用

## 1 相机系统

在游戏中，我们在屏幕中看到的视角通常会固定在角色身后，所以这里我们需要在角色身后绑定一个相机。

我们需要一个弹簧臂用来连接和相机，这里用UPROPERTY把它们暴露在蓝图中。

```c++
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
```

之后在构造函数里初始化它们：

大致三步操作：

1. 创建实例
2. 设置attach的地方
3. 其它参数设置

```c++
	//弹簧臂初始化
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f; //长度
	CameraBoom->bUsePawnControlRotation = true; //弹簧臂跟随controller旋转

	//相机初始化
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera222"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; //相机不跟随controller旋转
```

tips：

- USpringArmComponent::SocketName是自带的socket，位于弹簧臂末端

- RootComponent是每个Actor都自带的

## 2 输入系统

### 2.1 前后左右移动

首先在项目设置中添加Axis Mapping：

- MoveForward
  - W
  - S: Scale = -1
- MoveRight
  - D
  - A: Scale = -1

接下来在C++中绑定并应用这些操作

首先在头文件里定义两个函数分别用于前后/左右移动

```c++
	void MoveForward(float Value);
	void MoveRight(float Value);
```

函数体实现如下，首先获取Controller的**Yaw**旋转角度，再获取该角度(forward)下的**X方向**的单位向量，最后把Value添加到这个方向的输入中。MoveRight同理，只是换成了Y(Right)轴

```c++
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
```

最后在`SetupPlayerInputComponent`函数中绑定这两个函数到输入映射上。、

```c++
// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	//分别是项目设置中的映射名和c++中的函数名
	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
}
```



### 2.2 上下左右视角移动

#### 2.2.1 Tips

- Deltatime相关知识

  - **Delta Time**: 帧之间的间隔

  - **Frame**： 屏幕上更新的单张图像

  - **Frame Rate**： 每秒中更新的图像数量（FPS）

  - **Tick**：和frame一个意思

-  角色移动和帧的关系：假设actor每秒移动1cm，乘以deltatime就能得到每帧需要移动的长度

![](./imgs/Deltatime.png)

#### 2.2.2 使用方向键控制视角

首先在项目设置中添加Axis Mapping：

- TurnRate
  - →
  - ←: Scale = -1
- LookUpRate
  - ↑
  - ↓: Scale = -1

然后在头文件里定义两个变量，分别表示左右和上下的基础速率，在构造函数中初始化为45.f：

```c++
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
float BaseTurnRate; //单位是 度/秒

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
float BaseLookUpRate; //单位是 度/秒，
```

再定义两个函数处理左右和上下视角变换

```c++
  void TurnAtRate(float Rate); //Rate 介于0-1之间, 1代表100%是我们期望速率

  void LookUpAtRate(float Rate); //Rate 介于0-1之间
```

函数实现如下,里面计算的是当前帧需要改变的角度值,

```c++
void AShooterCharacter::TurnAtRate(float Rate)
{
	// deg/sec * sec/frame = deg/frame
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());

}
```

最后把函数绑定到axis mapping上

```c++
PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);
```

此时我们就能通过方向键控制角色的视角了。

#### 2.2.3 使用鼠标控制视角

> 当我们希望获得一致、恒定的转动速度时，就会使用DeltaTime。DeltaTime 可以补偿帧速率偏差，但当我们移动鼠标时，鼠标轴输入提供的是上一帧和这一帧之间的绝对延迟。我们并不希望这个转动速率是恒定的，而是希望它能准确反映我们自上一帧以来移动鼠标的次数。

|      | TurnAtRate                                                   | Turn                        |
| ---- | ------------------------------------------------------------ | --------------------------- |
| 区别 | 调用时使用rate，它结合了 delta time，可在按下按钮（开或关）时获得与帧速率无关的恒定运动。 | 从鼠标移动中获取 delta 值。 |

在项目设置里添加Turn和LookUp两个映射，对应鼠标X轴和Y轴。

之后再把它们与APawn自带的函数绑定

```c++
PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
```

> 使用下来没感觉到区别，，，

### 2.3 跳跃

创建Action Mapping

然后在C++中与角色类自带的Jump函数绑定

```c++
PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
```

## 3 动画系统
