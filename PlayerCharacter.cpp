// Preencher aviso de copyright no editor do Unreal.


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
#include "ZombieShooterCharacter.h"
#include "MainGameMode.h"
#include "ScreenPass.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

// Valores iniciais
APlayerCharacter::APlayerCharacter()
{
	// Ativar Tick() a cada frame; desligar para melhorar desempenho se nÃ£o for necessÃ¡rio.
	PrimaryActorTick.bCanEverTick = true;

	//Jogador não roda com a câmara
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//Braço da câmara e câmara em si
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom")); //Criação do braço
	CameraBoom->SetupAttachment(RootComponent); //Prende o braço ao componente raiz
	CameraBoom->TargetArmLength = 180.0f; //Distância da câmara
	CameraBoom->SocketOffset = FVector(0.0f, 60.0f, 70.0f); //Offset da câmara
	CameraBoom->bEnableCameraLag = true; //Dá um lag para dar peso ao movimento do jogador
	CameraBoom->CameraLagSpeed = 20.0f;
	CameraBoom->CameraRotationLagSpeed = 20.0f;
	CameraBoom->CameraLagMaxDistance = 10.0f;
	CameraBoom->bUsePawnControlRotation = true; //O braço roda com o rato
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera")); //Criação da câmara
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); //Prender a câmara ao braço
	FollowCamera->bUsePawnControlRotation = false; //A câmara não roda
	FollowCamera->FieldOfView = DefaultFOV; //Para ser mais cinemático
	
	
	GetCharacterMovement()->bOrientRotationToMovement = false; //O jogador não se move na direção do input
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); //Velocidade de rotação
	GetCharacterMovement()->JumpZVelocity = 500.0f; //Força do salto
	GetCharacterMovement()->AirControl = 0.35f; //Capacidade de movimento no ar
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; //Velocidade normal
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true; //Permite agachar
	
	
	//Vida atual
	CurrentHealth = MaxHealth;

	//Resistência atual
	CurrentStamina = MaxStamina;

	//Não está a sprintar
	bIsSprinting = false;

	//Não está a mirar
	bIsAiming = false;

	//Não está a recarregar
	bIsReloading = false;

	
	//Correção do crash, inicialização de ponteiros
	RifleRef = nullptr;
	PistolRef = nullptr;
	CurrentWeapon = nullptr;
    
	//Inicialização das classes, por segurança
	RifleClass = nullptr;
	PistolClass = nullptr;
}

// Chamado quando o jogo comeÃ§a ou quando o ator Ã© criado
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	
	//Mapping Context, caso o controlador seja válido
	if (APlayerController* PController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0); //Prioridade máxima
		}
	}


	//Criação das armas, se a classe de rifle for válida
	if (RifleClass)
	{
		//Parâmetros de spawn
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		RifleRef = GetWorld()->SpawnActor<ABaseWeapon>(RifleClass, GetActorLocation(), GetActorRotation(), SpawnParams);

		//Se a referência da rifle for válida
		if (RifleRef)
		{
			RifleRef->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("RifleSocket"));
		}
	}

	//Se a classe da pistola for válida
	if (PistolClass)
	{
		//Parâmetros de spawn
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		PistolRef = GetWorld()->SpawnActor<ABaseWeapon>(PistolClass, GetActorLocation(), GetActorRotation(), SpawnParams);

		//Se a referência da pistola for válida
		if (PistolRef)
		{
			PistolRef->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("PistolSocket"));
			PistolRef->SetActorHiddenInGame(true); //A pistola nasce escondida
		}
	}

	//Carrega os dados do save, caso a instância de jogo seja válida
	if (UShooterGameInstance* GameInst = Cast<UShooterGameInstance>(GetGameInstance()))
	{
		//Variáveis temporárias
		int32 SavedWave, SavedZombies, PrimMag, PrimRes, SecMag, SecRes;
		float SavedScore, SavedHealth;
		float SavedTime;
		bool bSavedIntermission;
		FVector SavedPlayerLocation;
		FRotator SavedPlayerRotation;
		TArray<FString> LoadedPickups; //Array para armazenar as pickups carregadas do progresso do jogo

		//A função devolve 'true' se o ficheiro de save existir e for válido
		if (GameInst->LoadCurrentProgress(SavedWave, SavedZombies, SavedScore, SavedHealth, PrimMag, PrimRes, SecMag, SecRes, SavedTime, bSavedIntermission, SavedPlayerLocation, SavedPlayerRotation, LoadedPickups))
		{
			CurrentHealth = SavedHealth;

			//Aplica as munições diretamente nas armas que acabaram de nascer
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

			//Mensagem de debug
			UE_LOG(LogTemp, Warning, TEXT("Valores do Save aplicados com sucesso no BeginPlay do Jogador!"));

			// Restaura a posição e rotação do jogador
			SetActorLocation(SavedPlayerLocation);
			SetActorRotation(SavedPlayerRotation);

			// Define a rotação do controlador para manter a orientação da câmara
			if (AController* C = GetController())
			{
				C->SetControlRotation(SavedPlayerRotation);
			}
		}
	}

	//Só tenta equipar se tivermos criado alguma arma com sucesso
	if (RifleRef)
	{
		EquipWeaponInternal(RifleRef, false);
	}
	else if (PistolRef) //Fallback se só tivermos pistola
	{
		EquipWeaponInternal(PistolRef, false);
	}
	
	//Guardar FOV se a câmara for válida
	if (FollowCamera)
	{
		DefaultFOV = FollowCamera->FieldOfView;
	}

	//Verifica se o jogador é controlado localmente e se escolhemos um widget
	if (IsLocallyControlled() && HUDWidgetClass)
	{
		//Cria o widget
		HUDWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		//Coloca no ecrã
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
		}
	}
}

// Chamado em cada frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Chama-se a função que lida com a resistência
	ManageStamina(DeltaTime);
}

// Liga aÃ§Ãµes aos inputs
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Liga as ações às funções C++
	if (UEnhancedInputComponent* EIComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Mover
		EIComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		
		//Olhar
		EIComp->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		//Saltar
		EIComp->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::CheckJump);
		EIComp->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Sprintar
		EIComp->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::StartSprint);
		EIComp->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopSprint);

		//Agachar
		EIComp->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::StartCrouch);
		EIComp->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopCrouch);

		//Disparar
		EIComp->BindAction(FireAction, ETriggerEvent::Started, this, &APlayerCharacter::StartWeaponFire);
		EIComp->BindAction(FireAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopWeaponFire);

		//Recarregar
		EIComp->BindAction(ReloadAction, ETriggerEvent::Started, this, &APlayerCharacter::ReloadWeapon);

		//Equipar espingarda
		EIComp->BindAction(EquipRifleAction, ETriggerEvent::Started, this, &APlayerCharacter::EquipRifle);

		//Equipar pistola
		EIComp->BindAction(EquipPistolAction, ETriggerEvent::Started, this, &APlayerCharacter::EquipPistol);

		//Mirar
		EIComp->BindAction(AimAction, ETriggerEvent::Started, this, &APlayerCharacter::StartAim);
		EIComp->BindAction(AimAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopAim);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	//Lê o valor do input
	FVector2D MoveVector = Value.Get<FVector2D>();

	//Se houver controlador
	if (Controller != nullptr)
	{
		//Descobre para onde a câmara está a apontar
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		//Obtém direção para a frente e direita
		const FVector FwdDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RgtDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		//Aplica movimento
		AddMovementInput(FwdDir, MoveVector.Y);
		AddMovementInput(RgtDir, MoveVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	//Lê o valor do input
	FVector2D LookVector = Value.Get<FVector2D>();

	//Se houver controlador
	if (Controller != nullptr)
	{
		//Aplica rotação
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void APlayerCharacter::StartSprint()
{
	//Se estiver a mirar, pára de o fazer
	if (bIsAiming)
	{
		StopAim();
	}

	//Não podemos sprintar agachado
	if (bIsCrouched)
	{
		StopCrouch();
	}

	if (CurrentStamina > 1.0f)
	{
		bIsSprinting = true;
		UpdateMovementSpeed(); //Atualiza a velocidade
	}
}

void APlayerCharacter::StopSprint()
{
	bIsSprinting = false;
	UpdateMovementSpeed(); //Volta ao normal
}

void APlayerCharacter::StartCrouch()
{
	//Se agachar, pára de correr
	if (bIsSprinting)
	{
		StopSprint();
	}

	Crouch(); //Função nativa do Unreal

	//Atualiza a velocidade
	UpdateMovementSpeed();
}

void APlayerCharacter::StopCrouch()
{
	UnCrouch(); //Função nativa do UE

	//Atualiza a velocidade
	UpdateMovementSpeed();
}

void APlayerCharacter::CheckJump()
{
	//Salta se tiver o mínimo necessário de resistência para o fazer
	if (CurrentStamina >= JumpStaminaCost)
	{
		CurrentStamina -= JumpStaminaCost;
		Jump(); //Função nativa do UE
	}
}

void APlayerCharacter::StartWeaponFire()
{
	//Se tivermos uma arma
	if (CurrentWeapon)
	{
		CurrentWeapon->PullTrigger();
	}
}

void APlayerCharacter::StopWeaponFire()
{
	//Se tivermos uma arma
	if (CurrentWeapon)
	{
		CurrentWeapon->ReleaseTrigger();
	}
}

void APlayerCharacter::ReloadWeapon()
{
	//Se já estiver a recarregar, ignora
	if (bIsReloading)
	{
		return;
	}
	
	//Verifica se temos arma e se o pente não está cheio
	if (CurrentWeapon && CurrentWeapon->CurrentAmmoInMag < CurrentWeapon->MaxAmmoInMag && CurrentWeapon->TotalAmmoReserve > 0)
	{
		//Pára de mirar quando se recarrega
		if (bIsAiming)
		{
			StopAim();
		}

		//Está a recarregar
		bIsReloading = true;

		//Valor de segurança
		float AnimDuration = 2.0f;
		
		//Toca as animações
		if (CurrentWeapon->ReloadMontage)
		{
			AnimDuration = PlayAnimMontage(CurrentWeapon->ReloadMontage);
		}

		if (CurrentWeapon->WeaponMesh && CurrentWeapon->WeaponReloadAnim)
		{
			CurrentWeapon->WeaponMesh->PlayAnimation(CurrentWeapon->WeaponReloadAnim, false);
		}

		//Definimos um temporizador para acabar o reload quando a animação acabar
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
	//Verificação de segurança
	if (!WeaponToEquip)
	{
		return;
	}

	//Se já temos esta arma equipada, não fazemos nada
	if (CurrentWeapon == WeaponToEquip)
	{
		return;
	}

	//Esconder arma antiga
	if (CurrentWeapon)
	{
		CurrentWeapon->SetActorHiddenInGame(true);
	}

	//Atualizar referência
	CurrentWeapon = WeaponToEquip;
	CurrentWeapon->SetActorHiddenInGame(false);

	//Linkar Layers de Animação
	if (CurrentWeapon->WeaponAnimLayer)
	{
		GetMesh()->LinkAnimClassLayers(CurrentWeapon->WeaponAnimLayer);
	}
    
	//Tocar Montage
	if (bPlayAnimation && CurrentWeapon->EquipMontage)
	{
		PlayAnimMontage(CurrentWeapon->EquipMontage);
	}
}

void APlayerCharacter::StartAim()
{
	//Se estiver a recarregar ou a sprintar não pode mirar
	if (bIsReloading || bIsSprinting)
	{
		return;
	}
	
	//Se estiver a sprintar pára para mirar
	if (bIsSprinting)
	{
		StopSprint();
	}

	//Está a mirar
	bIsAiming = true;
    
	//Atualiza a velocidade
	UpdateMovementSpeed();

	//Zoom
	if (FollowCamera)
	{
		FollowCamera->SetFieldOfView(50.0f); //Muda o FOV
	}
	if (CameraBoom) 
	{
		CameraBoom->TargetArmLength = 100.0f;
		CameraBoom->SocketOffset = FVector(0.0f, 40.0f, 60.0f); //Aproxima mais
	}
}

void APlayerCharacter::StopAim()
{
	//Não está a mirar
	bIsAiming = false;

	//Atualiza a velocidade
	UpdateMovementSpeed();

	//Reseta o zoom
	if (FollowCamera)
	{
		FollowCamera->SetFieldOfView(80.0f); //Reseta o FOV
	}
	if (CameraBoom) 
	{
		CameraBoom->TargetArmLength = 180.0f; //Valor base
		CameraBoom->SocketOffset = FVector(0.0f, 60.0f, 70.0f);
	}
}

void APlayerCharacter::FinishReload()
{
	bIsReloading = false;
    
	//Atualiza as balas
	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();
	}
}

void APlayerCharacter::UpdateMovementSpeed()
{
	//Valores de velocidade
	const float Speed_Stand_Hip = 600.0f;
	const float Speed_Stand_Aim = 300.0f;
	const float Speed_Crouch_Hip = 300.0f;
	const float Speed_Crouch_Aim = 150.0f;

	//Valor padrão
	float TargetSpeed = Speed_Stand_Hip;

	if (bIsCrouched)
	{
		//Se está agachado
		TargetSpeed = bIsAiming ? Speed_Crouch_Aim : Speed_Crouch_Hip;
	}
	else
	{
		//Se está de pé e a mirar
		if (bIsAiming)
		{
			TargetSpeed = Speed_Stand_Aim;
		}
		else if (bIsSprinting) //Se está a sprintar
		{
			TargetSpeed = SprintSpeed; 
		}
		else //Se só está de pé
		{
			TargetSpeed = Speed_Stand_Hip;
		}
	}

	//Atualiza o valor da velocidade
	GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;
}



void APlayerCharacter::ManageStamina(float DeltaTime)
{
	//Se estiver a correr e a mover-se
	if (bIsSprinting && GetVelocity().Size() > 0.0f) 
	{
		CurrentStamina -= SprintStaminaCost * DeltaTime;

		//Se a resistência se esgotar
		if (CurrentStamina <= 0.0f)
		{
			CurrentStamina = 0.0f;
			StopSprint(); //Força a paragem
		}
	}
	else if (CurrentStamina < MaxStamina) //Se a resistência atual for inferior à máxima
	{
		//Recupera resistência se não estiver a correr
		CurrentStamina += StaminaRegenRate * DeltaTime;

		//Se a resistência atual for igual ou superior à máxima
		if (CurrentStamina >= MaxStamina)
		{
			CurrentStamina = MaxStamina;
		}
	}
}

void APlayerCharacter::OnDeath_Implementation()
{
	//Log para debug
	UE_LOG(LogTemp, Error, TEXT("JOGADOR MORREU - GAME OVER"));

	//Desativa movimento se houver controlador válido
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		DisableInput(PC);
	}

	//Ativa o ragdoll
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                   class AController* EventInstigator, AActor* DamageCauser)
{
	//Chama a implementação base
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//Se já estiver morto, ignora
	if (CurrentHealth <= 0.0f)
	{
		return 0.0f;
	}

	// *** LÓGICA DE DIFICULDADE ***
	float DamageMultiplier = 1.0f;

	//Pedimos ao GameMode o multiplicador de dano dos inimigos
	if (AMainGameMode* GM = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode()))
	{
		DamageMultiplier = GM->EnemyDmgMult(); 
	}

	ActualDamage *= DamageMultiplier;
	//------------------------------------------

	//Retirar vida
	CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

	//Se a vida chegar a 0, Game Over
	if (CurrentHealth <= 0.0f)
	{
		//Avisar o GameMode (Permadeath)
		if (AMainGameMode* GM = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->OnPlayerDied();
		}
        
		//Lógica de Ragdoll
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
	if (CurrentHealth >= MaxHealth)
	{
		return false;
	}
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);

	return true;
}

void APlayerCharacter::AddAmmo(TSubclassOf<class ABaseWeapon> WeaponType, int32 Amount)
{
	//Verifica a arma equipada primeiro
	if (CurrentWeapon && CurrentWeapon->IsA(WeaponType))
	{
		CurrentWeapon->AddAmmo(Amount);
		return;
	}

	//Se não for para a arma da mão, verifica se é para a espingarda guardada
	if (RifleRef && RifleRef->IsA(WeaponType))
	{
		RifleRef->AddAmmo(Amount);
		UE_LOG(LogTemp, Log, TEXT("Munição adicionada à Espingarda (Inventário)"));
		return;
	}

	//Se não for, verifica se é para a pistola guardada
	if (PistolRef && PistolRef->IsA(WeaponType))
	{
		PistolRef->AddAmmo(Amount);
		UE_LOG(LogTemp, Log, TEXT("Munição adicionada à Pistola (Inventário)"));
		return;
	}
}

void APlayerCharacter::PlayFootstepSound()
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






