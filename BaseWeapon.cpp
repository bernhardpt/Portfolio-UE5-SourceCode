#include "BaseWeapon.h"
#include "ABaseProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Perception/AISense_Hearing.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	// Setup skeletal mesh as the root component to handle sockets and transform
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh")); 
	RootComponent = WeaponMesh; 
	
	// Initialize default ammo values
	CurrentAmmoInMag = 30; 
	TotalAmmoReserve = 120; 
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmoInMag = MaxAmmoInMag; 
}

void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseWeapon::PullTrigger()
{
	if (bIsAutomatic)
	{
		// Handle automatic fire loop via timer
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFire, this, &ABaseWeapon::Fire, FireRate, true, 0.0f); 
	}
	else
	{
		// Single shot for semi-automatic weapons
		Fire(); 
	}
}

void ABaseWeapon::ReleaseTrigger()
{
	// Stop automatic fire loop
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFire); 
}

void ABaseWeapon::Reload()
{
	// Prevent reload if already reloading, magazine is full, or no reserve ammo exists
	if (bIsReloading || CurrentAmmoInMag >= MaxAmmoInMag || TotalAmmoReserve <= 0) 
	{
		return; 
	}

	// Lock weapon state
	bIsReloading = true; 

	if (ReloadMontage)
	{
		if (ACharacter* MyOwner = Cast<ACharacter>(GetOwner()))
		{
			// Play reload animation on the owning character (relies on AnimNotifies to call FinishReloading)
			MyOwner->PlayAnimMontage(ReloadMontage); 
		}
	}
}

void ABaseWeapon::FinishReloading()
{
	// Unlock weapon state
	bIsReloading = false; 
	
	// Calculate required ammo and safely transfer from reserve to magazine
	int32 AmmoNeeded = MaxAmmoInMag - CurrentAmmoInMag; 
	int32 AmmoToLoad = FMath::Min(AmmoNeeded, TotalAmmoReserve); 

	CurrentAmmoInMag += AmmoToLoad; 
	TotalAmmoReserve -= AmmoToLoad; 
    
	UE_LOG(LogTemp, Log, TEXT("Reload Complete. Mag: %d | Reserve: %d"), CurrentAmmoInMag, TotalAmmoReserve); 
}

void ABaseWeapon::Fire()
{
	// Validate combat state and engine availability
    if (bIsReloading || !ProjectileClass || !GetWorld()) return; 

    if (!CanFire())
    {
        if (TotalAmmoReserve > 0)
        {
            Reload(); // Auto-reload if magazine is empty but reserve has ammo
        }
        else if (DryFireSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, GetActorLocation()); 
        }
        return; 
    }

	// Attempt to resolve the pawn owning or attached to this weapon
    APawn* MyOwner = Cast<APawn>(GetOwner()); 
    if (!MyOwner)
    {
        MyOwner = Cast<APawn>(GetAttachParentActor()); 
    }
    
    if (MyOwner)
    {
        AController* OwnerController = MyOwner->GetController(); 
        
        if (APlayerController* PC = Cast<APlayerController>(OwnerController))
        {
            FVector CamLoc; 
            FRotator CamRot;
            
			// Obtain player camera perspective for accurate raycasting
            PC->GetPlayerViewPoint(CamLoc, CamRot); 

			// Offset spawn location slightly forward to prevent clipping
            FVector SpawnLocation = CamLoc + (CamRot.Vector() * 100.0f); 
            FRotator SpawnRotation = CamRot; 

			// Configure spawn parameters for damage instigation and collision handling
            FActorSpawnParameters SpawnParams; 
            SpawnParams.Owner = MyOwner; 
            SpawnParams.Instigator = MyOwner; 
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; 

            AABaseProjectile* NewProjectile = GetWorld()->SpawnActor<AABaseProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams); 

            if (NewProjectile)
            {
				// Transfer weapon damage stats to the projectile
                NewProjectile->DamageValue = BaseDamage; 

                if (NewProjectile->ProjectileMovement)
                {
					// Apply ballistics data
                    NewProjectile->ProjectileMovement->InitialSpeed = BulletSpeed; 
                    NewProjectile->ProjectileMovement->MaxSpeed = BulletSpeed; 
                    NewProjectile->ProjectileMovement->Velocity = SpawnRotation.Vector() * BulletSpeed; 
                }

                if (BulletSpeed > 0.0f && EffectiveRange > 0.0f)
                {
					// Limit projectile lifespan based on effective range limits
                    float LifeSpan = (EffectiveRange * 100.0f) / BulletSpeed; 
                    NewProjectile->SetLifeSpan(LifeSpan); 
                }
            }
        }
    }

    if (WeaponMesh && WeaponFireAnim)
    {
        WeaponMesh->PlayAnimation(WeaponFireAnim, false); 
    }

	// Consume ammunition
    CurrentAmmoInMag--; 
    
	// Report noise event to AI perception system to alert enemies
    UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.0f, this, 0.0f, FName("Tiro")); 
}

bool ABaseWeapon::CanFire() const
{
	return CurrentAmmoInMag > 0; 
}

void ABaseWeapon::AddAmmo(int32 Amount)
{
	TotalAmmoReserve += Amount; 
}