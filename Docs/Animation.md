# 动画系统

> 官方系列教程：https://www.youtube.com/watch?v=ffuq5k-j0AY&t=3627s&ab_channel=UnrealEngine
>
> ​	part3：https://www.twitch.tv/videos/278640857

## 0 基础概念

- **Anim Montage**: 动画蒙太奇可包含一个或多个动画的资产。它至少有一个Section - 称为 "Default"。可以在蒙太奇时间轴的任意位置向蒙太奇添加新的Section。您可以随意移动、调整等。
- **Montage Section**: 动画蒙太奇的组成部分，用于将蒙太奇分割成可单独播放的片段。播放蒙太奇时，蒙太奇将从头开始播放。不过它并不会总是按顺序播放所有蒙太奇片段。哪些段落会在其他段落之后播放，取决于蒙太奇Section选项卡中的配置，可通过Window -> Montage Sections进行访问：
  - 在 C++ 中，使用 Montage_Play()播放蒙太奇，使用 Montage_JumpToSection()播放单个Section。
- **Slot**: 插槽Slot是一种标识符，是一种可以分配给动画蒙太奇的标签。当一个动画蒙太奇使用了一个特定的slot时，该slot可用于选择性地将pose信息添加到使用特定动画蓝图的角色中，具体做法是在动画 BP 中添加一个slot节点，并选择相关的slot名称。在播放蒙太奇时，动画蓝图中的pose将通过使用给定slot的slot节点 "添加 "到该姿势数据中。我在 "添加 "前加了引号，因为这取决于蒙太奇中的动画是否是Addictive的。
  - 如果不是Addictive，在播放时，姿势将被蒙太奇中的动画完全覆盖。
  - 如果这些动画是Addictive的，它们将被添加到通过槽节点输入的姿势数据之上。

### Blend 节点

> 官方文档：https://docs.unrealengine.com/4.26/en-US/AnimatingObjects/SkeletalMeshAnimation/NodeReference/Blend/

- **Layered Blend Per Bone**[官方讲解](https://youtu.be/ffuq5k-j0AY?t=3627): 假如我们想在跑步时开枪，此时只需要上半身开枪，下半身跑步，就可以用这个节点实现。

  - base pose 0是基础的运动动画

  - base pose 1是需要在某骨骼上混合的动画，例如开枪动画

  - Blend Weights是Additive pose的影响程度，1代表Additive完全添加到Base pose中，看源码`AnimationRuntime.cpp`这应该是混合的上限，如下。

  - ```c++
    float TargetBlendWeight = BlendWeights[PoseIndex] * SrcBoneBlendWeights[BoneIndex].BlendWeight;
    ```

  - Branch Filters: 这里设置混合的Bone Name和Blend Depth

    - Bone Name: 骨骼名，如pelvis, spine_01, thigh_01等等。

    - Blend Depth: 混合深度，代表每层深度添加的混合权重。源码中这么写的：

      - ```c++
        // how much weight increase Per depth
        const float IncreaseWeightPerDepth = (BranchFilter.BlendDepth != 0) ? (1.f/((float)BranchFilter.BlendDepth)) : 1.f;
        ```

    - 例如Bone Name = "pelvis"， Blend Depth = 4。此时每层添加的权重就是1/4 = 0.25。blend的顺序是pelvis，spine_01, spine_0, spine_03。到达spine_03后，它将播放100%混合后的开枪动画。其它的如pelvis只播放25%, spine_01播放50%, spine_02播放75%。

    - 当Blend Depth设置为-1时，表示不要blend这部分bone

## 1 C++基类创建

创建`UAnimInstance`的C++子类`UShooterAnimInstance`作为我们的动画蓝图基类。

在C++中，首先我们重写`UAnimInstance`自带的两个函数来初始化和更新动画所需变量。如下：

```c++
//每帧调用， 更新角色的运动状态变量
virtual void NativeUpdateAnimation(float DeltaSeconds) override;

//初始化函数
virtual void NativeInitializeAnimation() override;
```

之后我们前向声明角色类，用于获取角色当前状态。还要三个变量用来状态机的转换

```c++
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Moement, meta = (AllowPrivateAccess = "true"))
class AShooterCharacter* ShooterCharacter;

//角色当前速度
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess = "true"))
float Speed;

//角色是否在空中
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess = "true"))
bool bIsInAir;

// 判断角色是否在移动
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess = "true"))
bool bIsAccelerating;
```

角色指针初始化如下：

```c++
ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
```

Update函数里它们的更新逻辑如下：

```c++
if(ShooterCharacter)
{
    //更新速度
    FVector Velocity = ShooterCharacter->GetCharacterMovement()->Velocity;
    Velocity.Z = 0.f;
    Speed = Velocity.Size();

    //更新是否在空中
    bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

    //更新是否在移动
    bIsAccelerating =
        ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
}
```

## 2 动画蓝图创建

创建蓝图子类ABP_Shooter，创建基础状态机，使用刚刚的三个变量作为过渡条件，红圈地方可以设置为自动过渡。

可以通过裁剪动画，使得过渡更平滑

![](.\imgs\Locomotion.png)

## 添加开枪动画

想要的效果：上半身开枪，下半身跑步。

使用layered blend per bone节点，只需要在附加的pose前加上slot节点使得其可以被蒙太奇动画覆盖就可以实现。

节点中设置骨骼名为spine_01, 深度设置>=0即可。

