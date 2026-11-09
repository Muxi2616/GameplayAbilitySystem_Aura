# Aura

基于 **Unreal Engine 5.2** 开发的俯视角 ARPG 练习项目，当前分支面向 **Unreal Engine 5.8 + Rider** 适配。项目以 **Gameplay Ability System（GAS）** 为核心，实现完整的战斗、成长、技能管理、存档读档与关卡通关闭环。

原始 UE 5.2 版本保留在 `ue-5.2` 标签，升级开发使用 `codex/ue5.8-upgrade` 分支。

> 个人项目 / 面试作品：重点展示 GAS 战斗公式、技能状态机、存档系统设计与 C++ / Blueprint 分层架构。

---

## 项目简介

`Aura` 是第三人称俯视角 ARPG。玩家通过移动、施法与技能管理进行战斗与探索。项目采用 **C++ 负责底层规则，Blueprint 负责表现与流程扩展** 的混合开发方式，核心闭环包括：

- **战斗循环**：战斗 → 击杀 → 获得经验 → 升级成长  
- **成长循环**：属性加点、技能解锁 / 升级 / 装备  
- **持久化循环**：Checkpoint 存档、地图切换、读档恢复玩家与世界状态  
- **关卡目标**：Beacon 点亮计数，达标触发通关  

---

## 技术栈

| 类别     | 技术                                       |
| -------- | ------------------------------------------ |
| 引擎     | Unreal Engine 5.8（原版为 5.2）             |
| 核心框架 | Gameplay Ability System (GAS)              |
| 输入     | Enhanced Input + GameplayTag               |
| AI       | Behavior Tree + Blackboard                 |
| UI       | UMG + Widget Controller + MVVM             |
| 特效     | Niagara                                    |
| 存档     | SaveGame + `ISaveInterface` 世界状态序列化 |

---

## 已实现功能

### 1. 角色与成长

- 玩家移动、镜头、输入响应  
- 经验值与等级系统（XP / Level）  
- 升级奖励：Attribute Points / Spell Points  
- 属性体系：  
  - 主属性：Strength、Intelligence、Resilience、Vigor  
  - 次级属性：Armor、Armor Penetration、Block Chance、Critical Hit 等  
  - 抗性：Fire / Lightning / Arcane / Physical  
- 升级表现与数值刷新  

### 2. GAS 能力系统

- 自定义 `UAuraAbilitySystemComponent` 与 `UAuraAttributeSet`  
- 技能状态机：`Locked → Eligible → Unlocked → Equipped`  
- Spell Points 解锁 / 升级技能  
- 主动槽 + 被动槽装备与切换  
- 被动能力启停联动（含 Niagara 同步）  
- 通过 GameplayTag 统一组织与查询能力信息  

### 3. 战斗与伤害计算

- 多伤害类型：Fire / Lightning / Arcane / Physical  
- 完整伤害链路：抗性减伤 → 护甲与穿透 → 格挡 → 暴击  
- Debuff 系统：概率、持续时间、周期伤害、眩晕与输入阻断  
- 径向伤害（含衰减）  
- 受击反馈、死亡流程、伤害飘字  

### 4. 技能与特效

- 投射物技能：FireBolt（多弹 / 追踪）、FireBlast  
- Beam 技能：Electrocute（连锁目标，等级提升增加附加目标数）  
- 范围技能：ArcaneShards（多点落点）  
- 召唤技能框架  
- 被动光环 / Debuff Niagara 组件  
- 施法指示器（Magic Circle）  

### 5. 输入与交互

- Enhanced Input + GameplayTag 输入映射  
- Press / Hold / Release 触发能力  
- 鼠标点击移动 + Navigation 自动寻路  
- WASD 直接移动  
- 状态驱动输入限制（如 Stun）  
- Checkpoint / 敌人高亮与点击寻路目标点  

### 6. AI 与敌人

- 敌人角色 + AIController  
- 行为树驱动战斗行为  
- 最近目标搜索（BT Service）  
- 受击、眩晕、死亡、生命周期管理  
- 刷怪点 / 刷怪体积（状态可存档）  
- 敌人血条 UI  

### 7. UI 系统

- 主 HUD：生命、法力、经验、等级、提示消息  
- 法术菜单：技能选择、描述、升级、装备  
- 属性菜单：属性展示与加点  
- 伤害飘字  
- 加载界面（MVVM）：多槽位创建 / 选择 / 删除 / 开始游戏  

### 8. 存档与读档

- 多存档槽位管理  
- 玩家进度：等级、经验、点数、关键属性、技能状态 / 槽位 / 等级  
- 世界状态：通过 `ISaveInterface` 序列化关卡中可存档 Actor  
- Checkpoint 存档与地图入口切换  
- 读档后恢复玩家与世界状态（含已激活对象外观）  

### 9. 关卡通关（Beacon）

- 基于 Checkpoint 扩展的 Beacon Actor  
- 点亮计数、唯一 ID 去重、存档持久化  
- 达到指定数量触发 GameMode 通关事件  
- 可配置为「仅计数、不作重生点」  

---

## 架构亮点（面试可讲）

1. **GAS 驱动战斗**：自定义 `ExecCalc_Damage`，统一处理抗性、护甲、格挡、暴击与 Debuff 判定。  
2. **技能状态机解耦**：`Locked / Eligible / Unlocked / Equipped` + 槽位系统，便于扩展技能树。  
3. **双层存档**：玩家进度 + 世界 Actor 状态，读档一致性可验证。  
4. **UI 分层**：Widget Controller / MVVM 分离逻辑与表现。  
5. **接口解耦**：`ICombatInterface`、`ISaveInterface`、`IHighlightInterface`、`IPlayerInterface` 等，降低模块耦合。  
6. **数据驱动**：`CharacterClassInfo`、`AbilityInfo`、`LootTiers`、CurveTable 等 DataAsset 配置成长与掉落。  
7. **网络意识**：Server / Client / Multicast RPC、TargetData 复制、ASC Mixed/Minimal 复制策略（适合 Listen Server 联机战斗）。  

---

## 目录结构

```text
Source/Aura/
  AbilitySystem/   # ASC、AttributeSet、Abilities、ExecCalc、Data
  Character/       # 玩家与敌人
  Player/          # PlayerController、PlayerState
  AI/              # AIController、BT Service / Task
  UI/              # HUD、Widget、WidgetController、MVVM
  Game/            # GameMode、SaveGame、GameInstance
  Actor/           # 投射物、特效、刷怪、落点集合
  Checkpoint/      # 存档点、地图入口、Beacon
  Input/           # 输入配置
  Interaction/     # Combat / Save / Highlight 等接口
```

---

## 如何运行

**环境要求**

- Unreal Engine **5.8**，本机适配目标为 **5.8.4**。
- Rider；直接打开 `Aura.uproject`，无需先生成 `.sln`。
- Visual Studio 2026 / Build Tools 的 C++ 编译工具；也可使用 VS 2022 **17.14** 的对应工具链。Rider 本身不提供 MSVC，不需要使用 Visual Studio IDE 编写代码。
- Windows SDK **10.0.26100.0**。本机已使用 VS 2026 的 MSVC **14.51.36260** 完成编辑器构建；UE 5.8 对该工具链会提示其比首选版本更新，首选工具链为 MSVC 14.50 / 14.44。
- 使用 Visual Studio Installer 导入仓库根目录 `.vsconfig` 安装所需组件。

**步骤**

1. 检出 `codex/ue5.8-upgrade` 分支，安装上述编译工具。
2. 在 Rider 中打开 `Aura.uproject`，等待 Unreal 项目模型加载完成。
3. 使用 `AuraEditor / Development / Win64` 配置构建；首次完整构建时关闭 UE 编辑器。
4. 构建成功后启动 UE 编辑器，从 `LoadMenu` 创建并选择新存档，再进入游戏。
5. 编辑器中可将 `Editor Preferences > Source Code` 的源码编辑器设为 Rider。

也可以从项目根目录的 PowerShell 编译：

```powershell
$projectPath = (Resolve-Path .\Aura.uproject).Path
& 'C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat' `
    AuraEditor Win64 Development "-Project=$projectPath" -WaitMutex -NoHotReloadFromIDE
```

**Rider 提示 .NET Core installation folder not found**

在 `Settings > Build, Execution, Deployment > Toolset and Build`（中文界面：构建、执行、部署 > 工具包和构建）中，将 `.NET CLI executable path` 设置为引擎自带的工具：

```text
C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\ThirdParty\DotNet\10.0\win-x64\dotnet.exe
```

保存后重新加载项目。安装引擎的目录不同时，需要相应调整路径。这是本机的 Rider 设置，保存在已忽略提交的 `Aura.uproject.DotSettings.user`，不需要为此修改项目源码。2026-11-09 已在 Rider 中验证项目模型生成成功。

**RiderLink**

仓库自带的 RiderLink 为 2025.3.1，当前在 `.uproject` 中禁用，避免旧 IDE 插件阻碍升级。代码编辑、构建和调试仍可使用 Rider。需要蓝图交互等 RiderLink 功能时，通过当前 Rider 安装匹配版本，再启用 `.uproject` 中的 RiderLink；项目目录与引擎目录只保留一处安装。

**升级后的验证**

- 当前配置在编辑器中启动 `LoadMenu`，打包后启动 `MainMenu`，两条流程都应测试。
- 使用新存档验证创建 / 选择存档、进入关卡、移动 / 寻路、技能按下 / 持续 / 松开。
- 验证燃烧持续伤害、眩晕、输入阻断及效果到期后的恢复。
- 验证升级、技能装备、被动技能、检查点存档、重启读档、切图和死亡重生。
- C++ 构建成功后，编译相关 Blueprint / Widget，检查 MVVM 绑定，再打 Windows Development 包检查地图与资源加载。

2026-11-09 已使用 UE 5.8.4 + VS 2026 + Windows SDK 10.0.26100.0 完成以下验证：

- `AuraEditor / Development / Win64` 和 `Aura / Development / Win64` 均构建成功。
- 141 个项目蓝图全部编译通过，无蓝图编译错误、警告或加载失败。
- `LoadMenu`、`MainMenu`、`Dungeon`、`Dungeon_2`、`Dungeon_3` 均通过 NullRHI 无界面启动检查，正常退出，无脚本错误或空引用。
- 已迁移并保存七个菜单控件资源，修复菜单焦点设置；对需要由蓝图手动访问 ViewModel 的控件启用 MVVM 的 `Create View Without Bindings`，避免新版引擎省略视图而产生空引用。

无界面检查不覆盖实际渲染、鼠标操作、战斗和存档闭环，这些仍需在编辑器中进行完整交互验证。验证日志位于忽略提交的 `Saved/Logs/UE58-*.log`。

---

## 核心类速览

<details>
<summary>点击展开完整类列表</summary>


### 启动与 GAS 核心

| 类                            | 作用                                               |
| ----------------------------- | -------------------------------------------------- |
| `UAuraAssetManager`           | 启动时初始化原生 GameplayTags 与 GAS 全局数据      |
| `UAuraAbilitySystemComponent` | 技能输入、状态 / 槽位、被动、升级与存档恢复        |
| `UAuraAttributeSet`           | 属性与战斗结算（伤害、XP、Debuff、飘字）           |
| `UAuraAbilitySystemLibrary`   | 初始化属性、应用伤害、目标筛选、EffectContext 读写 |
| `UExecCalc_Damage`            | 伤害执行计算（抗性、护甲、格挡、暴击、Debuff）     |
| `UTargetDataUnderMouse`       | AbilityTask：鼠标目标数据并同步服务器              |

### 技能

| 类                           | 作用                                     |
| ---------------------------- | ---------------------------------------- |
| `UAuraGameplayAbility`       | 技能基类（描述、法力、冷却）             |
| `UAuraDamageGameplayAbility` | 伤害技能基类（Debuff / 击退 / 径向参数） |
| `UAuraFireBolt`              | 火球：多弹 / 追踪                        |
| `UAuraFireBlast`             | 火焰爆裂                                 |
| `UElectrocute`               | 闪电链：连锁目标 + 眩晕                  |
| `UArcaneShards`              | 奥术碎片：多点落点                       |
| `UAuraPassiveAbility`        | 被动技能基类                             |
| `UAuraSummonAbility`         | 召唤技能基类                             |

### 角色 / 玩家 / AI

| 类                             | 作用                                    |
| ------------------------------ | --------------------------------------- |
| `AAuraCharacterBase`           | 角色公共基类（武器、死亡、Debuff 特效） |
| `AAuraCharacter`               | 玩家（相机、升级、存档、HUD）           |
| `AAuraEnemy`                   | 敌人（AI、血条、掉落）                  |
| `AAuraPlayerController`        | 输入、高亮、寻路、技能转发              |
| `AAuraPlayerState`             | 等级 / XP / 点数，持有玩家 ASC          |
| `AAuraAIController`            | 行为树 AI                               |
| `UBTService_FindNearestPlayer` | 最近目标搜索                            |

### 存档 / 流程 / UI

| 类                                    | 作用                                 |
| ------------------------------------- | ------------------------------------ |
| `AAuraGameModeBase`                   | 存档槽、世界状态、地图跳转、通关事件 |
| `ULoadScreenSaveGame`                 | 存档数据（玩家 + 世界 + Beacon）     |
| `ACheckpoint`                         | 检查点 / Beacon                      |
| `AMapEntrance`                        | 地图入口切图                         |
| `UMVVM_LoadScreen` / `UMVVM_LoadSlot` | 加载界面 ViewModel                   |
| `UOverlayWidgetController`            | 主 HUD 控制器                        |
| `USpellMenuWidgetController`          | 技能菜单                             |
| `UAttributeMenuWidgetController`      | 属性菜单                             |

### 接口

| 接口                  | 作用                       |
| --------------------- | -------------------------- |
| `ICombatInterface`    | 等级、死亡、插槽、受击状态 |
| `IPlayerInterface`    | XP、等级、点数、存档       |
| `ISaveInterface`      | Actor 世界状态持久化       |
| `IHighlightInterface` | 高亮与点击寻路目标点       |
| `IEnemyInterface`     | 战斗目标读写               |

</details>

---

## 后续可扩展

- 职业分支与更深度技能树  
- 敌人行为树策略（巡逻、拉扯、技能决策）  
- 任务系统与更丰富关卡目标  
- 装备、词条与掉落深化  
 

---

## 说明

本项目为个人学习与面试作品，基于 UE5 GAS 课程体系实践与扩展，重点在于理解并落地完整 ARPG 核心系统，而非完整商业成品。
