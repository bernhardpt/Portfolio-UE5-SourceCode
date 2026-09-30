// Preencher aviso de copyright no editor do Unreal.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "ZombieCharacter.generated.h"

class UWidgetComponent;
class UAISenseConfig_Sight;

UCLASS()
class ZOMBIESHOOTER_API AZombieCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Valores iniciais for this character's properties
	AZombieCharacter();

protected:
	// Chamado quando o jogo comeÃ§a ou quando o ator Ã© criado
	virtual void BeginPlay() override;

public:	
	// Chamado em cada frame
	virtual void Tick(float DeltaTime) override;

	// Liga aÃ§Ãµes aos inputs
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	//*** ATRIBUTOS ***

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++ | Atributos")
	float MaxHealth = 100.0f; //Vida máxima

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Atributos")
	float CurrentHealth; //Vida atual
	
	
	//*** USER INTERFACE ***

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "C++ | UI")
	UWidgetComponent* HealthBarWidgetComp; //Barra de vida

	FTimerHandle TimerHandle_HideHealthBar; //Temporizador para a barra de vida
	void HideHealthBar(); //Função para lidar com a barra de vida

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override; //Função nativa do UE

	
	//*** ANIMAÇÕES ***

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Animações")
	UAnimMontage* DeathMontage; //Animação de morte
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Animações")
	UAnimMontage* AttackMontage; //Animação de ataque

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Combate")
	float AttackRange = 150.0f; //Alcance de ataque

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Animações")
	float AttackDamage = 20.0f; //Dano de ataque

	void PlayAttackAnimation(); //Função que a BT vai chamar para iniciar a animação

	UFUNCTION(BlueprintCallable, Category = "C++ | Combate")
	void OnAttackHit(); //Função que a animação vai chamar no momento do impacto

	bool bIsDead = false; //Para garantir que so morre uma vez

	void Die(); //Função para limpar a lógica da morte

	//O som do ataque
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Som")
	USoundBase* AttackSound;

	//O som de "Horde"
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Som")
	USoundBase* AmbientSound;

	//Temporizador para tocar o som de ambiente aleatoriamente
	FTimerHandle TimerHandle_AmbientSound;

	//Função que toca o som de ambiente
	void PlayAmbientSound();

	//Função chamada pela animação
	UFUNCTION(BlueprintCallable, Category = "C++ | Som")
	void PlayFootstepSound();

	//Sons para diferentes superfícies
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Som")
	USoundBase* FootstepSoundDefault; //Som genérico (obrigatório)
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Som")
	USoundBase* FootstepSoundDirt;
};




