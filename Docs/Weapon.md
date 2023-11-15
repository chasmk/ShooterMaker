# 1 Item

## 1.1 基础设置

作为武器的基类，主要包含以下组件:

- `USkeletalMeshComponent* ItemMesh`: 武器的mesh
- `UBoxComponent* CollisionBox`: 用于检测line trace碰撞来显示拾取UI
- `UWidgetComponent* PickupWidget`: 拾取UI

此外还有武器名，子弹数，稀有度等变量暴露至蓝图后再设置到widget里即可。

## 1.2 Item State

作为enum，标记item当前所处状态，有五种：

| 状态名             | 描述               |
| ------------------ | ------------------ |
| Eis_Pickup         | 待拾取状态         |
| Eis_EquipInterping | 拾取后在空中飞状态 |
| Eis_PickedUp       | 在背包中状态       |
| Eis_Equipped       | 拿在手中的状态     |
| Eis_Falling        | 扔出枪在空中状态   |

每次更新状态时都用switch节点设置item里组件的状态，如碰撞，可见等等

# 2 Base Weapon

Item的父类，

## 2.1 扔枪

- 首先获取武器的forward和right vector

- 使用`FVector.RotateAngleAxis`来旋转角度

- 最后用**`PrimitiveComponent.AddImpulse`**函数把武器发射出去

- 然后需要设置一个timer，设置武器飞的时间，再次期间需要在tick函数里**保持武器竖直**不倒下

  - ```c++
    FRotator(0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f);
    ```

### 2.2 拾取时飞行曲线

想要的效果：拾取武器时让其在空中以曲线飞到面前（放大），再向下飞（进入背包）同时缩小

- 首先获取武器当前位置和目标位置（根据相机的位置计算）

- 开始时在Item类开启一个Timer，期间武器处于`EItemState::Eis_EquipInterping`状态。

- Timer期间每帧更新Item的位置，旋转和Scale，使用`VInterpTp`和`RInterpTo`。

  - 其中位置的Z值单独用曲线设置，输入Timer已用时间，返回相应的值。注意这里是在Z的相对位置上的缩放值，不是世界坐标。

    - ```c++
      UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = true))
      UCurveFloat* ItemZCurve;//定义一个曲线，在蓝图中设置
      
      //根据Timer获取曲线上的值
      const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
      const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
      ```

  - Rotation是插值转到侧面面对我们即可

  - Scale也是用曲线实现。

    - ```c++
      const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
      SetActorScale3D(FVector(ScaleCurveValue));
      ```



最终效果：



![](./imgs/InterpItem.gif)
