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

