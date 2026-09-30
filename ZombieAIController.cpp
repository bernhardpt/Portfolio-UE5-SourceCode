#include "ZombieCharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrainComponent.h"
#include "ZombieAIController.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "MainGameMode.h"

AZombieCharacter::AZombieCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AZombieAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Setup floating health bar widget component
	HealthBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarComp"));
	HealthBarWidgetComp->SetupAttachment(GetRootComponent());
	HealthBarWidgetComp->SetWidgetSpace(EWidgetSpace::Screen); 
	HealthBarWidgetComp->SetDrawSize(FVector2D(100.0f, 10.0f));
	HealthBarWidgetComp->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f)); 
	HealthBarWidgetComp->SetVisibility(false); 
}

void AZombieCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;

	// Initialize ambient sound loop with a random initial offset to desynchronize hordes
	float RandomInterval = FMath::RandRange(2.0f, 10.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_AmbientSound, this, &AZombieCharacter::PlayAmbientSound, RandomInterval, false);
}

void AZombieCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AZombieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AZombieCharacter::HideHealthBar()
{
	HealthBarWidgetComp->SetVisibility(false);
}

float AZombieCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return 0.0f;

	float DamageToApply = DamageAmount;

	// Scale damage received based on GameMode difficulty settings
	if (AMainGameMode* GM = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode()))
	{
		DamageToApply *= GM->PlayerDmgMult();
	}

	float ActualDamage = Super::TakeDamage(DamageToApply, DamageEvent, EventInstigator, DamageCauser);
	CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);
    
	if (CurrentHealth <= 0.0f)
	{
		Die(); 
        
		if (AMainGameMode* GM = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->OnZombieKilled(EventInstigator);
		}
	}
	else
	{
		// Display health bar upon taking damage and reset hide timer
		if (HealthBarWidgetComp)
		{
			HealthBarWidgetComp->SetVisibility(true);
            
			GetWorldTimerManager().ClearTimer(TimerHandle_HideHealthBar);
			GetWorldTimerManager().SetTimer(TimerHandle_HideHealthBar, this, &AZombieCharacter::HideHealthBar, 3.0f, false);
		}
	}

	return ActualDamage;
}

void AZombieCharacter::PlayAttackAnimation()
{
	if (AttackMontage && !GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackMontage))
	{
		PlayAnimMontage(AttackMontage);

		if (AttackSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
		}
	}
}

void AZourceCharacter::OnAttackHit() {} // Placeholder for scope, actual logic below:

void AZombieCharacter::OnAttackHit()
{
	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (PlayerChar)
	{
		float Distance = FVector::Dist(GetActorLocation(), PlayerChar->GetActorLocation());

		if (Distance <= AttackRange)
		{
			UGameplayStatics::ApplyDamage(
				PlayerChar,
				AttackDamage,
				GetController(),
				this,
				UDamageType::StaticClass()
			);
		}
	}
}

void AZombieCharacter::Die()
{
	bIsDead = true;

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	// Halt AI behavior tree logic immediately
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (AIController->GetBrainComponent())
		{
			AIController->GetBrainComponent()->StopLogic("Dead");
		}
		AIController->StopMovement();
	}

	// Disable capsule collision so the player can walk over the corpse
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	HealthBarWidgetComp->SetVisibility(false);

	// Garbage collection: clean up corpse after 10 seconds
	SetLifeSpan(10.0f);
}

void AZombieCharacter::PlayAmbientSound()
{
	if (bIsDead) return;

	if (AmbientSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AmbientSound, GetActorLocation());
	}

	float NextInterval = FMath::RandRange(5.0f, 15.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_AmbientSound, this, &AZombieCharacter::PlayAmbientSound, NextInterval, false);
}

void AZombieCharacter::PlayFootstepSound()
{
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, 150); 

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bReturnPhysicalMaterial = true; 

	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	USoundBase* SoundToPlay = FootstepSoundDefault; 

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

	if (SoundToPlay)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, GetActorLocation(), 1.0f, FMath::RandRange(0.9f, 1.1f));
	}
}