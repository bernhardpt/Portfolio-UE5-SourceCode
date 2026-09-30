// Preencher aviso de copyright no editor do Unreal.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BaseWeapon.h"
#include "Blueprint/UserWidget.h"
#include "PlayerCharacter.generated.h"


//*** ENUMERAÇÃO PARA A DIFICULDADE ***

UENUM(BlueprintType)
enum class EGameDifficulty : uint8
{
    Easy    UMETA(DisplayName = "Fácil"),
    Normal  UMETA(DisplayName = "Normal"),
    Hard    UMETA(DisplayName = "Difícil")
};


UCLASS()
class ZOMBIESHOOTER_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Valores iniciais for this character's properties
	APlayerCharacter();

protected:
	// Chamado quando o jogo comeÃ§a ou quando o ator Ã© criado
	virtual void BeginPlay() override;

public:	
	// Chamado em cada frame
	virtual void Tick(float DeltaTime) override;

	// Liga aÃ§Ãµes aos inputs
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//*** CÂMARA ***

	//Braço da câmara
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="C++ | Câmara")
	class USpringArmComponent* CameraBoom;
	
	//Câmara de terceira pessoa
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="C++ | Câmara")
	class UCameraComponent* FollowCamera;


	//*** CONTROLOS ***

	//IMC
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Controlos")
	class UInputMappingContext* DefaultMappingContext;

	//Mover
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Controlos")
	class UInputAction* MoveAction;
	
	//Olhar
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Controlos")
	UInputAction* LookAction;

	//Saltar
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Controlos")
	UInputAction* JumpAction;

	//Sprintar
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Controlos")
	UInputAction* SprintAction;

	//Agachar
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Controlos")
	UInputAction* CrouchAction;

	//Disparar
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Controlos")
	UInputAction* FireAction;

	//Mirar
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Controlos")
	UInputAction* AimAction;

	//Recarregar
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Controlos")
	UInputAction* ReloadAction;

	//Equipar espingarda
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Controlos")
	UInputAction* EquipRifleAction;

	//Equipar pistola
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="C++ | Controlos")
	UInputAction* EquipPistolAction;

	/*
	 * COLOCAR AQUI A CONFIGURAÇÃO DO INPUT DO MENU DA PAUSA
	 */

	//*** FUNÇÕES DE CONTROLOS ***

	//Função para mover
	void Move(const FInputActionValue& Value);

	//Função para olhar
	void Look(const FInputActionValue& Value);

	//Funções para sprintar
	void StartSprint();
	void StopSprint();

	//Funções para agachar
	void StartCrouch();
	void StopCrouch();

	//Função personalizada de salto para o gasto de resistência
	void CheckJump();

	
	//*** SISTEMA DE ARMAS ***

	//Arma atual
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Combate")
	class ABaseWeapon* CurrentWeapon;

	//Referência para a espingarda
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C++ | Combate")
	ABaseWeapon* RifleRef;

	//Referência para a pistola
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C++ | Combate")
	ABaseWeapon* PistolRef;

	//Classe da espingarda
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++ | Combate")
	TSubclassOf<ABaseWeapon> RifleClass;

	//Classe da pistola
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++ | Combate")
	TSubclassOf<ABaseWeapon> PistolClass;

	//Função de começo de disparo
	void StartWeaponFire();

	//Função de fim de disparo
	void StopWeaponFire();

	//Função de recarga
	void ReloadWeapon();

	//Função para equipar a espingarda (input)
	void EquipRifle();

	//Função para equipar a pistola (input)
	void EquipPistol();

	//Função que trata de equipar fisicamente as armas
	void EquipWeaponInternal(ABaseWeapon* WeaponToEquip, bool bPlayAnimation);
	
	//Função de começo de mira
	void StartAim();

	//Função de fim de mira
	void StopAim();

	//Variável para guardar o FOV original
	float DefaultFOV = 80.0f;

	//Variável para saber se o jogador está a mirar
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Combate")
	bool bIsAiming;

	//Variável para saber se está a recarregar
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Combate")
	bool bIsReloading;

	//Temporizador para lidar com a recarga
	FTimerHandle TimerHandle_Reload;

	//Função para lidar com o fim da animação de recarga
	void FinishReload();

	//Função central para alterar a velocidade
	void UpdateMovementSpeed();

	
	//*** ATRIBUTOS ***

	//Vida máxima
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++ | Atributos")
	float MaxHealth = 100.0f;

	//Vida atual
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Atributos")
	float CurrentHealth;

	//Resistência máxima
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++ | Atributos")
	float MaxStamina = 100.0f;

	//Resistência atual
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Atributos")
	float CurrentStamina;

	//Gasto de resistência por segundo a sprintar
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Atributos")
	float SprintStaminaCost = 20.0f;

	//Recuperação de resistência por segundo
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Atributos")
	float StaminaRegenRate = 10.0f;

	//Gasto de resistência por salto
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Atributos")
	float JumpStaminaCost = 10.0f;

	//Velocidade normal
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Movimento")
	float WalkSpeed = 600.0f;

	//Velocidade de sprint
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Movimento")
	float SprintSpeed = 900.0f;

	//Variável de controlo para ver se o jogador está a sprintar
	bool bIsSprinting;

	//Função para lidar com a resistência
	void ManageStamina(float DeltaTime);


	//*** SISTEMA DE DANO ***

	//Variável para armazenar a dificuldade atual
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++ | Configuração do Jogo")
	EGameDifficulty CurrentDifficulty = EGameDifficulty::Normal;

	//Função chamada quando o jogador morre
	UFUNCTION(BlueprintNativeEvent, Category = "C++ | Combate")
	void OnDeath();
	
	//Override da função nativa do UE
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	
	//*** HUD ***

	//Função para saber que arma está na mão
	UFUNCTION(BlueprintCallable, Category = "C++ | Combate")
	ABaseWeapon* GetCurrentWeapon() const;

	//Variável para escolher o Widget no Editor (WBP_HUD)
	UPROPERTY(EditDefaultsOnly, Category = "C++ | UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	//Referência para o widget criado
	UPROPERTY()
	UUserWidget* HUDWidgetInstance;


	//*** PICKUPS ***

	//Função para Curar
	bool Heal(float Amount);

	//Função para receber munição
	void AddAmmo(TSubclassOf<class ABaseWeapon> WeaponType, int32 Amount);

	//Função chamada pela animação
	UFUNCTION(BlueprintCallable, Category = "C++ | Som")
	void PlayFootstepSound();

	//Sons para diferentes superfícies
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Som")
	USoundBase* FootstepSoundDefault; //Som genérico (obrigatório)
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Som")
	USoundBase* FootstepSoundDirt;
};




