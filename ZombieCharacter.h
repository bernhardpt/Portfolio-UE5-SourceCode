#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "ZombieCharacter.generated.h"

class UWidgetComponent;

UCLASS()
class ZOMBIESHOOTER_API AZombieCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AZombieCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// --- ATTRIBUTES ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++ | Attributes")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Attributes")
	float CurrentHealth;
	
	// --- UI & HEALTH BAR ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "C++ | UI")
	UWidgetComponent* HealthBarWidgetComp; 

	FTimerHandle TimerHandle_HideHealthBar; 
	void HideHealthBar(); 

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// --- COMBAT & ANIMATION ---
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Animations")
	UAnimMontage* DeathMontage; 
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Animations")
	UAnimMontage* AttackMontage; 

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Combat")
	float AttackRange = 150.0f; 

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Combat")
	float AttackDamage = 20.0f; 

	void PlayAttackAnimation(); 

	UFUNCTION(BlueprintCallable, Category = "C++ | Combat")
	void OnAttackHit(); 

	bool bIsDead = false; 
	void Die(); 

	// --- AUDIO ---
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Audio")
	USoundBase* AttackSound;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Audio")
	USoundBase* AmbientSound;

	FTimerHandle TimerHandle_AmbientSound;
	void PlayAmbientSound();

	UFUNCTION(BlueprintCallable, Category = "C++ | Audio")
	void PlayFootstepSound();

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Audio")
	USoundBase* FootstepSoundDefault; 
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Audio")
	USoundBase* FootstepSoundDirt;
};