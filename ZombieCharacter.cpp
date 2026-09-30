// Preencher aviso de copyright no editor do Unreal.


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

// Valores iniciais
AZombieCharacter::AZombieCharacter()
{
 	// Ativar Tick() a cada frame; desligar para melhorar desempenho se nÃ£o for necessÃ¡rio.
	PrimaryActorTick.bCanEverTick = true;


	//*** CONFIGURAÇÃO DO AICONTROLLER ***

	AIControllerClass = AZombieAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;


	//*** BARRA DE VIDA ***

	HealthBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarComp"));
	HealthBarWidgetComp->SetupAttachment(GetRootComponent());
	HealthBarWidgetComp->SetWidgetSpace(EWidgetSpace::Screen); //Faz a barra olhar sempre para a câmara
	HealthBarWidgetComp->SetDrawSize(FVector2D(100.0f, 10.0f));
	HealthBarWidgetComp->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f)); //Acima da cabeça
	HealthBarWidgetComp->SetVisibility(false); //Começa invisível

}

// Chamado quando o jogo comeÃ§a ou quando o ator Ã© criado
void AZombieCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	//Iniciar o ciclo de sons de ambiente
	//Define um tempo aleatório inicial para não começarem todos ao mesmo tempo
	float RandomInterval = FMath::RandRange(2.0f, 10.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_AmbientSound, this, &AZombieCharacter::PlayAmbientSound, RandomInterval, false);
}

// Chamado em cada frame
void AZombieCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Liga aÃ§Ãµes aos inputs
void AZombieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AZombieCharacter::HideHealthBar()
{
	HealthBarWidgetComp->SetVisibility(false);
}

float AZombieCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	//Se já estiver morto, não leva mais dano
	if (bIsDead) return 0.0f;

	float DamageToApply = DamageAmount;

	// *** LÓGICA DE DIFICULDADE ***
	//Pedimos ao GameMode o multiplicador de dano do jogador
	if (AMainGameMode* GM = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode()))
	{
		DamageToApply *= GM->PlayerDmgMult();
	}
	//------------------------------------------

	//Aplica o dano calculado
	float ActualDamage = Super::TakeDamage(DamageToApply, DamageEvent, EventInstigator, DamageCauser);
    
	//Atualiza a vida
	CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);
    
	//Atualiza a UI da barra de vida (se tiveres a função)
	//UpdateHealthUI(); 

	if (CurrentHealth <= 0.0f)
	{
		Die(); //Chama a função de morte
        
		//Avisar o GameMode para dar pontos
		if (AMainGameMode* GM = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->OnZombieKilled(EventInstigator);
		}
	}
	else
	{
		//Lógica da barra de vida
		if (HealthBarWidgetComp)
		{
			HealthBarWidgetComp->SetVisibility(true);
            
			//Reinicia o temporizador para esconder a barra daqui a 3 segundos
			GetWorldTimerManager().ClearTimer(TimerHandle_HideHealthBar);
			GetWorldTimerManager().SetTimer(TimerHandle_HideHealthBar, this, &AZombieCharacter::HideHealthBar, 3.0f, false);
		}
	}

	return ActualDamage;
}

void AZombieCharacter::PlayAttackAnimation()
{
	//Se houver animação de ataque e ela não estiver já a ser tocada
	if (AttackMontage && !GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackMontage))
	{
		PlayAnimMontage(AttackMontage);

		//Tocar som de ataque
		if (AttackSound)
		{
			//Toca o som na localização da boca do zombie
			UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
		}
	}
}

void AZombieCharacter::OnAttackHit()
{
	//Esta função corre no frame exato em que a mão desce
    
	//Verificamos a distância ao jogador
	//Uma forma simples é verificar a distância ao Player 0
	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	//Se o jogador existir
	if (PlayerChar)
	{
		float Distance = FVector::Dist(GetActorLocation(), PlayerChar->GetActorLocation());

		//Se o jogador estiver perto e à frente do zombie
		if (Distance <= AttackRange)
		{
			//Aplicamos dano
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

	//Tocar animação de morte se for válida
	if (DeathMontage)
	{
		//Toca a montage e garante que ela não se repete
		PlayAnimMontage(DeathMontage);
	}

	//Parar o controlador AI
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		//Pára o cérebro imediatamente
		if (AIController->GetBrainComponent())
		{
			AIController->GetBrainComponent()->StopLogic("Dead");
		}
		AIController->StopMovement();
	}

	//Desliga a colisão e o movimento
	//Remove a cápsula física para o jogador poder andar por cima do corpo
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	//Impede que o zombie deslize ou seja empurrado
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	//Limpa o UI
	HealthBarWidgetComp->SetVisibility(false);

	//Faz o corpo desaparecer após 10 segundos
	SetLifeSpan(10.0f);
}

void AZombieCharacter::PlayAmbientSound()
{
	//Só toca se não estiver morto
	if (bIsDead) return;

	if (AmbientSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AmbientSound, GetActorLocation());
	}

	//Reinicia o timer com um novo tempo aleatório
	//Assim eles nunca gemem em uníssono
	float NextInterval = FMath::RandRange(5.0f, 15.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_AmbientSound, this, &AZombieCharacter::PlayAmbientSound, NextInterval, false);
}

void AZombieCharacter::PlayFootstepSound()
{
	//Raycast para baixo (ver onde está o pé)
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, 150); // 1.5 metros para baixo

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bReturnPhysicalMaterial = true; //Queremos saber o material.

	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	USoundBase* SoundToPlay = FootstepSoundDefault; //Som padrão

	//Se tocou no chão, verifica o material
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

	//Toca o som (com ligeira variação de pitch para não parecer robótico)
	if (SoundToPlay)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, GetActorLocation(), 1.0f, FMath::RandRange(0.9f, 1.1f));
	}
}





