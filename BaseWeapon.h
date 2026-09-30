#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

class AABaseProjectile;
class USoundBase;
class UAnimMontage;
class UAnimSequence;

UCLASS()
class ZOMBIESHOOTER_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseWeapon();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Visual representation of the weapon containing sockets for FX and projectiles
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Components")
	USkeletalMeshComponent* WeaponMesh;	
	
	// Core combat configuration
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Configuration")
	TSubclassOf<AABaseProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Configuration")
	FName MuzzleSocketName = "MuzzleFlash";

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Configuration")
	float BaseDamage = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Configuration")
	float FireRate = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Configuration")
	bool bIsAutomatic = false;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Audio")
	USoundBase* DryFireSound;

	// Ammo management and tracking
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++ | Ammo")
	int32 MaxAmmoInMag = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C++ | Ammo")
	int32 CurrentAmmoInMag;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C++ | Ammo")
	int32 TotalAmmoReserve;

	// Input handling for firing mechanisms
	void PullTrigger(); 
	void ReleaseTrigger(); 

	// Reload logic
	void Reload();

	UFUNCTION(BlueprintCallable, Category="C++ | Combat")
	void FinishReloading();

	// Core firing execution
	virtual void Fire();

	FTimerHandle TimerHandle_HandleFire;
	bool CanFire() const;

	// State tracking
	bool bIsReloading;

	// Animation references for both the character and the weapon mesh
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Player Animation")
	TSubclassOf<UAnimInstance> WeaponAnimLayer;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Player Animation")
	UAnimMontage* EquipMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Player Animation")
	UAnimMontage* ReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Weapon Animation")
	UAnimSequence* WeaponFireAnim;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Weapon Animation")
	UAnimSequence* WeaponReloadAnim;
	
	// Ballistics configuration for projectile physical behavior
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Ballistics")
	float EffectiveRange = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Ballistics")
	float BulletSpeed = 80000.0f;
	
	// Helper function for ammo pickups
	void AddAmmo(int32 Amount);
};