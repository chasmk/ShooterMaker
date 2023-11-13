# Item

作为武器的基类，主要包含以下组件:

- `USkeletalMeshComponent* ItemMesh`: 武器的mesh
- `UBoxComponent* CollisionBox`: 用于检测line trace碰撞来显示拾取UI
- `UWidgetComponent* PickupWidget`: 拾取UI

此外还有武器名，子弹数，稀有度等变量暴露至蓝图后再设置到widget里即可。

# Base Weapon

Item的父类，