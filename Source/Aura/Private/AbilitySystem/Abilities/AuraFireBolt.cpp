// Copyright  HuJiaHao


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			//Title
			"<Title>火球术: </>\n\n"
			
			//Level
			"<Small>等级: </><Level>%d</>\n"
			
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			
			//Cooldown
			"<Small>冷却时间: </><Cooldown>%.1f</>\n\n"
			
			"<Default>发射一个火球, "
			"在撞击后爆炸: </>"
			
			//Damage
			"<Damage>%d</><Default> 火焰伤害并且概率点燃敌人</>"), 
			
			//Values
			Level,
			ManaCost,
			Cooldown,
			ScaledDamage);
	}
	else
	{
		return FString::Printf(TEXT(
			//Title
			"<Title>火球术: </>\n\n"
			
			//Level
			"<Small>等级: </><Level>%d</>\n"
			
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			
			//Cooldown
			"<Small>冷却时间: </><Cooldown>%.1f</>\n\n"
			
			"<Default>发射 %d 个火球, "
			"在撞击后爆炸: </>"
			
			//Damage
			"<Damage>%d</><Default> 火焰伤害并且概率点燃敌人</>"), 
			
			//Values
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level, NumProjectiles),
			ScaledDamage);
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(TEXT(
			//Title
			"<Title>升级后: </>\n\n"
			
			//Level
			"<Small>等级: </><Level>%d</>\n"
			
			//ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			
			//Cooldown
			"<Small>冷却: </><Cooldown>%.1f</>\n\n"
			
			"<Default>发射 %d 个火球, "
			"在撞击后爆炸: </>"
			
			//Damage
			"<Damage>%d</><Default> 火焰伤害并且概率点燃敌人</>"), 
			
			//Values
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level, NumProjectiles),
			ScaledDamage);
	
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,
	bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
		GetAvatarActorFromActorInfo(),
		SocketTag);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch) Rotation.Pitch = PitchOverride;

	const FVector Forward = Rotation.Vector();
	const int32 EffectiveNumProjectiles = FMath::Min(NumProjectiles, GetAbilityLevel());
	TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, EffectiveNumProjectiles);

	for (const FRotator& Rot : Rotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
		{
			Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		}
		else
		{
			Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
			Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
		}

		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
		Projectile->ProjectileMovement->bIsHomingProjectile = bLaunchHomingProjectiles;
		
		
		Projectile->FinishSpawning(SpawnTransform);
	}
}
