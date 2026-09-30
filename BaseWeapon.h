// Preencher aviso de copyright no editor do Unreal.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

// Declarações de classes para evitar include desnecessário. Isso ajuda a reduzir o tempo de compilação, já que não precisamos incluir os arquivos completos dessas classes aqui, apenas dizer que elas existem.
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

	// Componentes da arma. O WeaponMesh é a representação visual da arma, onde ficam os sockets para spawnar os projéteis e efeitos. Ele deve ser configurado para usar uma mesh adequada (ex: um modelo de rifle) e para ter colisão desativada, já que a colisão é tratada pelos projéteis.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Componentes")
	USkeletalMeshComponent* WeaponMesh;	
	
	// Configurações da arma. O ProjectileClass é a classe do projétil que será spawnada ao disparar. O MuzzleSocketName é o nome do socket na mesh onde o projétil será spawnado. O BaseDamage é o dano base que o projétil causará, que pode ser modificado por outros fatores (ex: distância, armadura). O FireRate é a taxa de disparo para armas automáticas, ou seja, o tempo entre cada disparo. O bIsAutomatic indica se a arma é automática (true) ou semiautomática (false). O DryFireSound é o som que será tocado quando o jogador tentar disparar sem munição.
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Configurações")
	TSubclassOf<AABaseProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Configurações")
	FName MuzzleSocketName = "MuzzleFlash";

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Configurações")
	float BaseDamage = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Configurações")
	float FireRate = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Configurações")
	bool bIsAutomatic = false;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Áudio")
	USoundBase* DryFireSound;

	// Variáveis de munição. O MaxAmmoInMag é a capacidade máxima do pente, ou seja, quantas balas cabem no pente. O CurrentAmmoInMag é a quantidade atual de munição no pente, que é reduzida a cada disparo e aumentada a cada recarga. O TotalAmmoReserve é a quantidade total de munição de reserva que o jogador tem, que é usada para recarregar o pente quando ele fica vazio. O jogador pode ter uma quantidade limitada de munição de reserva, ou pode ter munição infinita (definida por um valor alto ou por uma flag separada).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "C++ | Munições")
	int32 MaxAmmoInMag = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C++ | Munições")
	int32 CurrentAmmoInMag;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C++ | Munições")
	int32 TotalAmmoReserve;

	// Funções de combate. PullTrigger é chamada quando o jogador pressiona o botão de disparo, e inicia o processo de disparo. Para armas automáticas, isso inicia um loop de disparo que continua enquanto o botão estiver pressionado. Para armas semiautomáticas, isso dispara uma vez. ReleaseTrigger é chamada quando o jogador solta o botão de disparo, e para o loop de disparo em armas automáticas. Reload é chamada para iniciar a recarga da arma, e FinishReloading é chamada no final da animação de recarga para atualizar a munição. Fire é a função que realmente executa o disparo, spawnando o projétil e aplicando os efeitos visuais e sonoros. CanFire é uma função auxiliar que verifica se a arma pode disparar (ex: se há munição no pente, se não está recarregando, etc).
	void PullTrigger(); 

	void ReleaseTrigger(); 

	void Reload();

	UFUNCTION(BlueprintCallable, Category="C++ | Combate")
	void FinishReloading();

	virtual void Fire();

	FTimerHandle TimerHandle_HandleFire;

	bool CanFire() const;

	// Variáveis de animação. O WeaponAnimLayer é a camada de animação que a arma usa para tocar suas animações (ex: recarga, disparo). O EquipMontage é a animação de equipar a arma, que pode ser tocada quando o jogador pega a arma ou troca de arma. O ReloadMontage é a animação de recarga da arma, que deve ter um notify no final para chamar a função FinishReloading(). O WeaponFireAnim é a animação de disparo da arma, que pode ser tocada a cada disparo para dar feedback visual. O WeaponReloadAnim é uma animação alternativa de recarga que pode ser usada em vez do ReloadMontage, dependendo do design da arma e do personagem. As animações devem ser configuradas para usar os sockets corretos na mesh da arma, e para ter as notificações necessárias para sincronizar os eventos de recarga e disparo.
	bool bIsReloading;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Animação do Jogador")
	TSubclassOf<UAnimInstance> WeaponAnimLayer;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Animação do Jogador")
	UAnimMontage* EquipMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Animação do Jogador")
	UAnimMontage* ReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Animação da Arma")
	UAnimSequence* WeaponFireAnim;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Animação da Arma")
	UAnimSequence* WeaponReloadAnim;
	
	// Variáveis de balística. O EffectiveRange é a distância máxima em que a arma é eficaz, ou seja, onde o dano começa a cair. O BulletSpeed é a velocidade inicial do projétil ao ser disparado, que afeta o tempo que leva para atingir o alvo e a trajetória do projétil. Essas variáveis podem ser usadas para calcular o dano real causado pelo projétil com base na distância ao alvo, e para ajustar a física do projétil durante o voo.
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Balística")
	float EffectiveRange = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "C++ | Balística")
	float BulletSpeed = 80000.0f;
	
	// Função para adicionar munição à reserva.
	void AddAmmo(int32 Amount);
	
};




