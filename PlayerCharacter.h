#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BaseWeapon.h"
#include "Blueprint/UserWidget.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class EGameDifficulty : uint8
{
    Easy    UMETA(DisplayName = "Easy"),
    Normal  UMETA(DisplayName = "Normal"),
    Hard    UMETA(DisplayName = "Hard")
};

UCLASS()
class ZOMBIESHOOTER_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// --- CAMERA COMPONENTS ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="C++ | Camera")
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="C++ | Camera")
	class UCameraComponent* FollowCamera;

	// --- ENHANCED INPUT SYSTEM ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Input")
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Input")
	class UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Input")
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Input")
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Input")
	UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Input")
	UInputAction* EquipRifleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Input")
	UInputAction* EquipPistolAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartSprint();
	void StopSprint();
	void StartCrouch();
	void StopCrouch();
	void CheckJump();

	// --- COMBAT & INVENTORY SYSTEM ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Combat")
	class ABaseWeapon* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C++ | Combat")
	ABaseWeapon* RifleRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C++ | Combat")
	ABaseWeapon* PistolRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++ | Combat")
	TSubclassOf<ABaseWeapon> RifleClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++ | Combat")
	TSubclassOf<ABaseWeapon> PistolClass;

	void StartWeaponFire();
	void StopWeaponFire();
	void ReloadWeapon();
	void EquipRifle();
	void EquipPistol();
	void EquipWeaponInternal(ABaseWeapon* WeaponToEquip, bool bPlayAnimation);
	void StartAim();
	void StopAim();
	void FinishReload();

	float DefaultFOV = 80.0f;
	FTimerHandle TimerHandle_Reload;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Combat")
	bool bIsAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Combat")
	bool bIsReloading;

	void UpdateMovementSpeed();

	// --- PLAYER ATTRIBUTES ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++ | Attributes")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Attributes")
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++ | Attributes")
	float MaxStamina = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Attributes")
	float CurrentStamina;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Attributes")
	float SprintStaminaCost = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Attributes")
	float StaminaRegenRate = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Attributes")
	float JumpStaminaCost = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Movement")
	float WalkSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Movement")
	float SprintSpeed = 900.0f;

	bool bIsSprinting;
	void ManageStamina(float DeltaTime);

	// --- DAMAGE & HEALTH SYSTEM ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++ | Config")
	EGameDifficulty CurrentDifficulty = EGameDifficulty::Normal;

	UFUNCTION(BlueprintNativeEvent, Category = "C++ | Combat")
	void OnDeath();
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// --- HUD ---
	UFUNCTION(BlueprintCallable, Category = "C++ | Combat")
	ABaseWeapon* GetCurrentWeapon() const;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY()
	UUserWidget* HUDWidgetInstance;

	// --- PICKUP INTERACTIONS ---
	bool Heal(float Amount);
	void AddAmmo(TSubclassOf<class ABaseWeapon> WeaponType, int32 Amount);

	// --- AUDIO SYSTEM ---
	UFUNCTION(BlueprintCallable, Category = "C++ | Audio")
	void PlayFootstepSound();

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Audio")
	USoundBase* FootstepSoundDefault; 
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Audio")
	USoundBase* FootstepSoundDirt;
};