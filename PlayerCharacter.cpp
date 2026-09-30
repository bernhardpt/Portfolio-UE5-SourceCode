#include "PlayerCharacter.h"
#include "BaseWeapon.h"
#include "ShooterGameInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MainGameMode.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Disconnect controller pitch/roll from character rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Setup Camera Boom (Spring Arm) for third-person perspective
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom")); 
	CameraBoom->SetupAttachment(RootComponent); 
	CameraBoom->TargetArmLength = 180.0f; 
	CameraBoom->SocketOffset = FVector(0.0f, 60.0f, 70.0f); 
	CameraBoom->bEnableCameraLag = true; 
	CameraBoom->CameraLagSpeed = 20.0f;
	CameraBoom->CameraRotationLagSpeed = 20.0f;
	CameraBoom->CameraLagMaxDistance = 10.0f;
	CameraBoom->bUsePawnControlRotation = true; 

	// Setup Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera")); 
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 
	FollowCamera->FieldOfView = DefaultFOV; 
	
	// Configure Movement Component parameters
	GetCharacterMovement()->bOrientRotationToMovement = false; 
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 
	GetCharacterMovement()->JumpZVelocity = 500.0f; 
	GetCharacterMovement()->AirControl = 0.35f; 
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; 
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true; 
	
	// Initialize core attributes
	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
	bIsSprinting = false;
	bIsAiming = false;
	bIsReloading = false;

	// Safe pointer initialization
	RifleRef = nullptr;
	PistolRef = nullptr;
	CurrentWeapon = nullptr;
	RifleClass = nullptr;
	PistolClass = nullptr;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Bind Enhanced Input Mapping Context to the local player
	if (APlayerController* PController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0); 
		}
	}

	// Instantiate and attach initial inventory weapons to skeletal sockets
	if (RifleClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		RifleRef = GetWorld()->SpawnActor<ABaseWeapon>(RifleClass, GetActorLocation(), GetActorRotation(), SpawnParams);

		if (RifleRef)
		{
			RifleRef->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("RifleSocket"));
		}
	}

	if (PistolClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		PistolRef = GetWorld()->SpawnActor<ABaseWeapon>(PistolClass, GetActorLocation(), GetActorRotation(), SpawnParams);

		if (PistolRef)
		{
			PistolRef->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("PistolSocket"));
			PistolRef->SetActorHiddenInGame(true); 
		}
	}

	// Reconstruct player state from save file if available
	if (UShooterGameInstance* GameInst = Cast<UShooterGameInstance>(GetGameInstance()))
	{
		int32 SavedWave, SavedZombies, PrimMag, PrimRes, SecMag, SecRes;
		float SavedScore, SavedHealth, SavedTime;
		bool bSavedIntermission;
		FVector SavedPlayerLocation;
		FRotator SavedPlayerRotation;
		TArray<FString> LoadedPickups; 

		if (GameInst->LoadCurrentProgress(SavedWave, SavedZombies, SavedScore, SavedHealth, PrimMag, PrimRes, SecMag, SecRes, SavedTime, bSavedIntermission, SavedPlayerLocation, SavedPlayerRotation, LoadedPickups))
		{
			CurrentHealth = SavedHealth;

			if (RifleRef)
			{
				RifleRef->CurrentAmmoInMag = PrimMag;
				RifleRef->TotalAmmoReserve = PrimRes;
			}
			if (PistolRef)
			{
				PistolRef->CurrentAmmoInMag = SecMag;
				PistolRef->TotalAmmoReserve = SecRes;
			}

			// Restore world position and camera orientation
			SetActorLocation(SavedPlayerLocation);
			SetActorRotation(SavedPlayerRotation);

			if (AController* C = GetController())
			{
				C->SetControlRotation(SavedPlayerRotation);
			}
		}
	}

	// Equip priority weapon on startup
	if (RifleRef)
	{
		EquipWeaponInternal(RifleRef, false);
	}
	else if (PistolRef) 
	{
		EquipWeaponInternal(PistolRef, false);
	}
	
	if (FollowCamera)
	{
		DefaultFOV = FollowCamera->FieldOfView;
	}

	// Instantiate and display HUD for local player
	if (IsLocallyControlled() && HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ManageStamina(DeltaTime);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EIComp->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EIComp->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::CheckJump);
		EIComp->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EIComp->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::StartSprint);
		EIComp->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopSprint);
		EIComp->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::StartCrouch);
		EIComp->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopCrouch);
		EIComp->BindAction(FireAction, ETriggerEvent::Started, this, &APlayerCharacter::StartWeaponFire);
		EIComp->BindAction(FireAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopWeaponFire);
		EIComp->BindAction(ReloadAction, ETriggerEvent::Started, this, &APlayerCharacter::ReloadWeapon);
		EIComp->BindAction(EquipRifleAction, ETriggerEvent::Started, this, &APlayerCharacter::EquipRifle);
		EIComp->BindAction(EquipPistolAction, ETriggerEvent::Started, this, &APlayerCharacter::EquipPistol);
		EIComp->BindAction(AimAction, ETriggerEvent::Started, this, &APlayerCharacter::StartAim);
		EIComp->BindAction(AimAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopAim);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MoveVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector FwdDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RgtDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(FwdDir, MoveVector.Y);
		AddMovementInput(RgtDir, MoveVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void APlayerCharacter::StartSprint()
{
	if (bIsAiming) StopAim();
	if (bIsCrouched) StopCrouch();

	if (CurrentStamina > 1.0f)
	{
		bIsSprinting = true;
		UpdateMovementSpeed(); 
	}
}

void APlayerCharacter::StopSprint()
{
	bIsSprinting = false;
	UpdateMovementSpeed(); 
}

void APlayerCharacter::StartCrouch()
{
	if (bIsSprinting) StopSprint();
	
	Crouch(); 
	UpdateMovementSpeed();
}

void APlayerCharacter::StopCrouch()
{
	UnCrouch(); 
	UpdateMovementSpeed();
}

void APlayerCharacter::CheckJump()
{
	if (CurrentStamina >= JumpStaminaCost)
	{
		CurrentStamina -= JumpStaminaCost;
		Jump(); 
	}
}

void APlayerCharacter::StartWeaponFire()
{
	if (CurrentWeapon) CurrentWeapon->PullTrigger();
}

void APlayerCharacter::StopWeaponFire()
{
	if (CurrentWeapon) CurrentWeapon->ReleaseTrigger();
}

void APlayerCharacter::ReloadWeapon()
{
	if (bIsReloading) return;
	
	if (CurrentWeapon && CurrentWeapon->CurrentAmmoInMag < CurrentWeapon->MaxAmmoInMag && CurrentWeapon->TotalAmmoReserve > 0)
	{
		if (bIsAiming) StopAim();

		bIsReloading = true;
		float AnimDuration = 2.0f;
		
		if (CurrentWeapon->ReloadMontage)
		{
			AnimDuration = PlayAnimMontage(CurrentWeapon->ReloadMontage);
		}

		if (CurrentWeapon->WeaponMesh && CurrentWeapon->WeaponReloadAnim)
		{
			CurrentWeapon->WeaponMesh->PlayAnimation(CurrentWeapon->WeaponReloadAnim, false);
		}

		// Delegate reload completion to timer
		GetWorldTimerManager().SetTimer(TimerHandle_Reload, this, &APlayerCharacter::FinishReload, AnimDuration, false);
	}
}

void APlayerCharacter::EquipRifle()
{
	EquipWeaponInternal(RifleRef, true);
}

void APlayerCharacter::EquipPistol()
{
	EquipWeaponInternal(PistolRef, true);
}

void APlayerCharacter::EquipWeaponInternal(ABaseWeapon* WeaponToEquip, bool bPlayAnimation)
{
	if (!WeaponToEquip || CurrentWeapon == WeaponToEquip) return;

	if (CurrentWeapon)
	{
		CurrentWeapon->SetActorHiddenInGame(true);
	}

	CurrentWeapon = WeaponToEquip;
	CurrentWeapon->SetActorHiddenInGame(false);

	// Dynamically link weapon-specific animation layers to the player mesh
	if (CurrentWeapon->WeaponAnimLayer)
	{
		GetMesh()->LinkAnimClassLayers(CurrentWeapon->WeaponAnimLayer);
	}
    
	if (bPlayAnimation && CurrentWeapon->EquipMontage)
	{
		PlayAnimMontage(CurrentWeapon->EquipMontage);
	}
}

void APlayerCharacter::StartAim()
{
	if (bIsReloading || bIsSprinting) return;
	if (bIsSprinting) StopSprint();

	bIsAiming = true;
	UpdateMovementSpeed();

	// Apply camera zoom logic
	if (FollowCamera) FollowCamera->SetFieldOfView(50.0f); 
	if (CameraBoom) 
	{
		CameraBoom->TargetArmLength = 100.0f;
		CameraBoom->SocketOffset = FVector(0.0f, 40.0f, 60.0f); 
	}
}

void APlayerCharacter::StopAim()
{
	bIsAiming = false;
	UpdateMovementSpeed();

	// Restore default camera perspective
	if (FollowCamera) FollowCamera->SetFieldOfView(80.0f); 
	if (CameraBoom) 
	{
		CameraBoom->TargetArmLength = 180.0f; 
		CameraBoom->SocketOffset = FVector(0.0f, 60.0f, 70.0f);
	}
}

void APlayerCharacter::FinishReload()
{
	bIsReloading = false;
	if (CurrentWeapon) CurrentWeapon->Reload();
}

void APlayerCharacter::UpdateMovementSpeed()
{
	const float Speed_Stand_Hip = 600.0f;
	const float Speed_Stand_Aim = 300.0f;
	const float Speed_Crouch_Hip = 300.0f;
	const float Speed_Crouch_Aim = 150.0f;

	float TargetSpeed = Speed_Stand_Hip;

	if (bIsCrouched)
	{
		TargetSpeed = bIsAiming ? Speed_Crouch_Aim : Speed_Crouch_Hip;
	}
	else
	{
		if (bIsAiming)
		{
			TargetSpeed = Speed_Stand_Aim;
		}
		else if (bIsSprinting) 
		{
			TargetSpeed = SprintSpeed; 
		}
	}

	GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;
}

void APlayerCharacter::ManageStamina(float DeltaTime)
{
	if (bIsSprinting && GetVelocity().Size() > 0.0f) 
	{
		CurrentStamina -= SprintStaminaCost * DeltaTime;
		if (CurrentStamina <= 0.0f)
		{
			CurrentStamina = 0.0f;
			StopSprint(); 
		}
	}
	else if (CurrentStamina < MaxStamina) 
	{
		CurrentStamina += StaminaRegenRate * DeltaTime;
		if (CurrentStamina >= MaxStamina)
		{
			CurrentStamina = MaxStamina;
		}
	}
}

void APlayerCharacter::OnDeath_Implementation()
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		DisableInput(PC);
	}

	// Trigger physics ragdoll
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (CurrentHealth <= 0.0f) return 0.0f;

	// Scale incoming damage based on GameMode difficulty settings
	float DamageMultiplier = 1.0f;
	if (AMainGameMode* GM = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode()))
	{
		DamageMultiplier = GM->EnemyDmgMult(); 
	}

	ActualDamage *= DamageMultiplier;
	CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

	// Handle death sequence
	if (CurrentHealth <= 0.0f)
	{
		if (AMainGameMode* GM = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->OnPlayerDied();
		}
        
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	}

	return ActualDamage;
}

ABaseWeapon* APlayerCharacter::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

bool APlayerCharacter::Heal(float Amount)
{
	if (CurrentHealth >= MaxHealth) return false;
	
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
	return true;
}

void APlayerCharacter::AddAmmo(TSubclassOf<class ABaseWeapon> WeaponType, int32 Amount)
{
	if (CurrentWeapon && CurrentWeapon->IsA(WeaponType))
	{
		CurrentWeapon->AddAmmo(Amount);
		return;
	}

	if (RifleRef && RifleRef->IsA(WeaponType))
	{
		RifleRef->AddAmmo(Amount);
		return;
	}

	if (PistolRef && PistolRef->IsA(WeaponType))
	{
		PistolRef->AddAmmo(Amount);
		return;
	}
}

void APlayerCharacter::PlayFootstepSound()
{
	// Raycast downward to detect floor material for dynamic audio
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, 150); 

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bReturnPhysicalMaterial = true; 

	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	USoundBase* SoundToPlay = FootstepSoundDefault; 

	// Override default sound based on Physical Surface Type
	if (bHit && Hit.PhysMaterial.IsValid())
	{
		EPhysicalSurface SurfaceType = Hit.PhysMaterial->SurfaceType;

		switch (SurfaceType)
		{
		case SurfaceType1:
			SoundToPlay = FootstepSoundDefault; 
			break;
		case SurfaceType2:
			if (FootstepSoundDirt) SoundToPlay = FootstepSoundDirt;
			break;
		default: 
			SoundToPlay = FootstepSoundDefault;
			break;
		}
	}

	// Play spatialized sound with slight pitch randomization to prevent audio fatigue
	if (SoundToPlay)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, GetActorLocation(), 1.0f, FMath::RandRange(0.9f, 1.1f));
	}
}