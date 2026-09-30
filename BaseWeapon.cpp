// Preencher aviso de copyright no editor do Unreal.


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

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh")); //Cria o componente de mesh da arma
	RootComponent = WeaponMesh; //Torna a mesh o componente base do ator. Assim, os sockets ficam na mesh e a posição/rotação da arma é controlada por ela
	
	CurrentAmmoInMag = 30; //Munição inicial no pente
	TotalAmmoReserve = 120; //Munição inicial de reserva
	
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmoInMag = MaxAmmoInMag; //Começa com o pente cheio. O valor pode ser modificado no editor para armas específicas, mas por padrão começa cheio.
	
}

void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseWeapon::PullTrigger()
{
	if (bIsAutomatic)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFire, this, &ABaseWeapon::Fire, FireRate, true, 0.0f); //Inicia o temporizador para chamar a função Fire a cada FireRate segundos. O último parâmetro (0.0f) é o tempo para o primeiro disparo, ou seja, dispara imediatamente.
		
	}
	else
	{
		Fire(); //Dispara uma vez para armas semiautomáticas
		
	}
	
}

void ABaseWeapon::ReleaseTrigger()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFire); //Limpa o temporizador para parar o loop de disparo em armas automáticas. Para armas semiautomáticas, isso não tem efeito, mas é seguro chamar mesmo assim.
	
}

void ABaseWeapon::Reload()
{
	if (bIsReloading || CurrentAmmoInMag >= MaxAmmoInMag || TotalAmmoReserve <= 0) 
	{
		return; //Validações para impedir recarga se já estiver recarregando, se o pente já estiver cheio ou se não houver munição de reserva
		
	}


	bIsReloading = true; //Tranca a arma para impedir outras ações durante a recarga

	if (ReloadMontage)
	{
		if (ACharacter* MyOwner = Cast<ACharacter>(GetOwner()))
		{
			MyOwner->PlayAnimMontage(ReloadMontage); //Toca a animação de recarga no personagem dono da arma. A animação deve ter um notify no final que chama a função FinishReloading() para atualizar os valores de munição.
		}
		
	}
	
}

void ABaseWeapon::FinishReloading()
{
	bIsReloading = false; //Destranca a arma para permitir outras ações após a recarga
	
	int32 AmmoNeeded = MaxAmmoInMag - CurrentAmmoInMag; //Calcula quantas balas precisamos para encher o pente (pode ser menos do que isso se já tivermos algumas balas no pente)
    
	int32 AmmoToLoad = FMath::Min(AmmoNeeded, TotalAmmoReserve); //Calcula quantas balas vamos carregar, que é o mínimo entre o que precisamos para encher o pente e o que temos de reserva. Por exemplo, se precisamos de 10 balas para encher o pente, mas só temos 5 de reserva, só carregamos 5.

	CurrentAmmoInMag += AmmoToLoad; //Adiciona as balas carregadas ao pente
	TotalAmmoReserve -= AmmoToLoad; //Subtrai as balas carregadas da reserva
    
	UE_LOG(LogTemp, Log, TEXT("Recarga completa! Pente: %d | Bolso: %d"), CurrentAmmoInMag, TotalAmmoReserve); //Mensagem de debug para verificar os valores após a recarga
	
}

void ABaseWeapon::Fire()
{
    if (bIsReloading || !ProjectileClass || !GetWorld()) return; //Validações para impedir disparo durante recarga, se a classe do projétil não estiver definida ou se o mundo não estiver disponível (caso raro, mas é bom validar)

    if (!CanFire())
    {
        if (TotalAmmoReserve > 0)
        {
            Reload(); //Se não houver munição no pente, mas houver na reserva, inicia a recarga automaticamente. O jogador pode optar por recarregar manualmente antes de tentar disparar, ou pode simplesmente tentar disparar e deixar o sistema cuidar da recarga.
        }
        else
        {
            if (DryFireSound)
            {
                UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, GetActorLocation()); //Toca o som de gatilho seco na localização da arma. Pode ser ajustado para tocar na localização do jogador ou em outro local, se desejado.
            	
            }
        	
        }
        
        return; //Sai da função para impedir o disparo quando não há munição
    	
    }


    APawn* MyOwner = Cast<APawn>(GetOwner()); //Tenta obter o personagem dono da arma. A arma pode estar anexada a um personagem ou a um actor que é filho de um personagem, então tentamos ambos.
    
    if (!MyOwner)
    {
        MyOwner = Cast<APawn>(GetAttachParentActor()); //Tenta obter o personagem dono da arma a partir do actor ao qual a arma está anexada. Isso é útil para casos onde a arma está anexada a um personagem, mas o dono direto da arma é um actor intermediário (por exemplo, um "WeaponHolder" que é filho do personagem). Se a arma estiver diretamente anexada ao personagem, isso ainda funcionará porque o personagem é o attach parent.

    }

    
    if (MyOwner)
    {
        AController* OwnerController = MyOwner->GetController(); //Obtém o controlador do personagem dono da arma. O controlador é necessário para obter a localização e rotação da câmera para o raycast, e também para configurar o instigador do dano corretamente.
        
        if (APlayerController* PC = Cast<APlayerController>(OwnerController))
        {
            FVector CamLoc; //Variáveis para armazenar a localização e rotação da câmera do jogador
            FRotator CamRot;
            PC->GetPlayerViewPoint(CamLoc, CamRot); //Obtém a localização e rotação da câmera do jogador. Isso é importante para o raycast, que determina onde o jogador está mirando, e para ajustar a direção do disparo para apontar para o alvo.

            FVector SpawnLocation = CamLoc + (CamRot.Vector() * 100.0f); //Calcula uma localização de spawn inicial para o projétil, que é um pouco à frente da câmera. Isso é útil para evitar que o projétil colida com o jogador ou a arma imediatamente ao ser spawnado, e também para garantir que o raycast comece a partir da perspectiva do jogador.

            FRotator SpawnRotation = CamRot; //Inicialmente, a rotação de spawn do projétil é a mesma da câmera, para que o raycast seja feito na direção que o jogador está olhando.

            FActorSpawnParameters SpawnParams; //Configura os parâmetros de spawn para o projétil. Isso inclui o dono e instigador para o sistema de dano, e a configuração de colisão para garantir que o projétil sempre spawna mesmo que haja algo no local de spawn.
            SpawnParams.Owner = MyOwner; //O dono do projétil é o personagem dono da arma. Isso é importante para o sistema de dano, para que o jogo saiba quem causou o dano quando o projétil atingir algo.
            SpawnParams.Instigator = MyOwner; //O instigador do projétil também é o personagem dono da arma. Isso é importante para o sistema de dano, para que o jogo saiba quem causou o dano quando o projétil atingir algo. O instigador é usado principalmente para determinar a equipe do personagem e aplicar regras de dano (por exemplo, não causar dano a aliados).
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; //Configura o método de tratamento de colisão para o spawn do projétil. "AlwaysSpawn" garante que o projétil seja criado mesmo que haja algo no local de spawn, o que é importante para evitar falhas no disparo. O projétil deve ser configurado para lidar com colisões corretamente (por exemplo, destruindo-se ao colidir com algo), para que isso não cause problemas.

            AABaseProjectile* NewProjectile = GetWorld()->SpawnActor<AABaseProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams); //Spawna o projétil na localização do cano da arma, com a rotação calculada para apontar para o alvo, e com os parâmetros de spawn configurados. O tipo do projétil é definido pela variável ProjectileClass, que deve ser configurada no editor para cada arma específica.

            if (NewProjectile)
            {
                NewProjectile->DamageValue = BaseDamage; //Configura o valor de dano do projétil para o valor base definido na arma. O projétil pode usar esse valor para aplicar dano quando colidir com algo.

                if (NewProjectile->ProjectileMovement)
                {
                    NewProjectile->ProjectileMovement->InitialSpeed = BulletSpeed; //Configura a velocidade inicial do projétil para o valor definido na arma. O componente de movimento do projétil deve ser configurado para usar essa velocidade inicial para mover o projétil na direção da sua rotação.
                    NewProjectile->ProjectileMovement->MaxSpeed = BulletSpeed; //Configura a velocidade máxima do projétil para o mesmo valor da velocidade inicial, para garantir que o projétil mantenha uma velocidade constante. Se a velocidade máxima fosse maior, o projétil poderia acelerar além da velocidade desejada, o que pode não ser desejável para armas de fogo.
                    NewProjectile->ProjectileMovement->Velocity = SpawnRotation.Vector() * BulletSpeed; //Configura a velocidade do projétil para ser na direção da rotação de spawn (que já aponta para o alvo) multiplicada pela velocidade da bala. Isso garante que o projétil se mova na direção correta com a velocidade correta desde o momento em que é spawnado.
                    
                }

                if (BulletSpeed > 0.0f && EffectiveRange > 0.0f)
                {
                    float LifeSpan = (EffectiveRange * 100.0f) / BulletSpeed; //Calcula o tempo de vida do projétil com base no alcance efetivo e na velocidade da bala. O alcance efetivo é multiplicado por 100 para converter de metros para centímetros, que é a unidade padrão do Unreal. O tempo de vida é o tempo que o projétil levaria para percorrer o alcance efetivo à velocidade definida. Isso garante que o projétil seja destruído automaticamente após percorrer a distância máxima eficaz, para evitar que ele continue existindo indefinidamente e cause problemas de desempenho ou jogabilidade.
                    NewProjectile->SetLifeSpan(LifeSpan); //Configura o tempo de vida do projétil para o valor calculado. O projétil será destruído automaticamente quando esse tempo expirar, o que ajuda a manter o desempenho do jogo e a evitar que projéteis perdidos continuem existindo indefinidamente.
                    
                }

            }
        	
        }
    	
    }

    if (WeaponMesh && WeaponFireAnim)
    {
        WeaponMesh->PlayAnimation(WeaponFireAnim, false); //Toca a animação de disparo na mesh da arma. A animação deve ser configurada para não ser em loop, para que ela toque apenas uma vez a cada disparo.
    	
    }

    CurrentAmmoInMag--; //Subtrai uma bala do pente a cada disparo. Isso deve ser feito no final da função para garantir que o disparo aconteça mesmo quando o pente está vazio (por exemplo, para permitir o disparo do último projétil antes de ficar sem munição).
    
    UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.0f, this, 0.0f, FName("Tiro")); //Relata um evento de ruído para o sistema de percepção de IA, para que os inimigos possam reagir ao som do disparo. O local do evento é a localização da arma, a intensidade é 1.0f (pode ser ajustada para armas mais silenciosas ou mais barulhentas), o instigador é a própria arma (pode ser ajustado para o personagem dono, se desejado), o alcance é 0.0f (sem limite, mas ainda limitado pelo alcance dos sensores dos inimigos), e a tag é "Tiro" para identificar o tipo de ruído.
	
}

bool ABaseWeapon::CanFire() const
{
	return CurrentAmmoInMag > 0; //Permite disparar somente se houver munição no pente. O jogador pode recarregar quando o pente estiver vazio, mas não pode disparar até recarregar.
	
}

void ABaseWeapon::AddAmmo(int32 Amount)
{
	TotalAmmoReserve += Amount; //Adiciona a quantidade especificada à munição de reserva. Essa função pode ser chamada por pickups de munição, por exemplo, para aumentar a munição do jogador.
	
}
