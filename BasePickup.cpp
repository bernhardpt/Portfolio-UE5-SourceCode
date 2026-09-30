// Preencher aviso de copyright no editor do Unreal.


#include "BasePickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "PlayerCharacter.h"

ABasePickup::ABasePickup()
{
	PrimaryActorTick.bCanEverTick = true;

	// Configuração dos componentes do item coletável. O SphereComp é o componente de colisão usado para detectar quando o jogador entra na área de coleta, e o RotatingComp é usado para fazer o item girar lentamente, o que é um efeito visual comum para itens coletáveis. A MeshComp é a representação visual do item, e está configurada para não ter colisão, já que a detecção de coleta é feita pelo SphereComp.
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(50.0f);
	SphereComp->SetCollisionProfileName("Trigger");
	RootComponent = SphereComp;

	// A mesh do item é configurada para ser um componente filho do SphereComp, o que significa que ela seguirá a posição e rotação do SphereComp. A colisão da mesh é desativada para evitar interferências com a detecção de coleta, já que o SphereComp é responsável por isso. A mesh pode ser configurada no editor para cada tipo de item específico, permitindo uma variedade de aparências para os itens coletáveis no jogo.
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// O componente de movimento rotativo é usado para fazer o item girar lentamente, o que é um efeito visual comum para itens coletáveis. Ele pode ser configurado para controlar a velocidade e o eixo de rotação, e é uma maneira fácil de adicionar um pouco de vida ao item no mundo. Neste caso, o item gira em torno do eixo Y a uma velocidade de 90 graus por segundo, o que cria um efeito de rotação suave e visualmente agradável.
	RotatingComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComp"));
	RotatingComp->RotationRate = FRotator(0.0f, 90.0f, 0.0f);
}

void ABasePickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Função para lidar com a lógica de quando o item é coletado pelo jogador. Ela recebe um ponteiro para o personagem do jogador que coletou o item, o que permite aplicar efeitos ou dar recompensas ao jogador. Esta função deve ser implementada para definir o comportamento específico do item quando ele é coletado. As classes filhas implementarão esta função para definir o que acontece quando o jogador apanha o item. Pode ser usado para dar vida, munição, armas, etc.
void ABasePickup::OnPickup(class APlayerCharacter* Player)
{
	// As classes filhas implementarão esta função para definir o que acontece quando o jogador apanha o item. Pode ser usado para dar vida, munição, armas, etc.

}

// Função para lidar com a lógica de quando o item é coletado pelo jogador. Ela é chamada automaticamente pelo sistema de colisão do Unreal quando outro ator entra na área de colisão definida pelo SphereComp. O OtherActor é o ator que colidiu com o SphereComp, e podemos verificar se é o personagem do jogador para determinar se devemos chamar a função OnPickup. Esta função é importante para garantir que a lógica de coleta seja acionada corretamente quando o jogador interage com o item no mundo.
void ABasePickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// Verifica se o item está ativo antes de processar a colisão. Se o item não estiver ativo, ele não deve ser coletado, e a função retorna imediatamente. Isso é útil para itens que devem desaparecer temporariamente após serem coletados, ou para controlar a disponibilidade do item no mundo. Se o item estiver ativo, a função continua para verificar se o ator que colidiu é o personagem do jogador.
	if (!bIsActive) return;
	
	Super::NotifyActorBeginOverlap(OtherActor);

	// Verifica se o ator que colidiu é o personagem do jogador usando a função Cast. Se a colisão for com o personagem do jogador, a função OnPickup é chamada, passando um ponteiro para o personagem do jogador que coletou o item. Isso permite que a lógica de coleta seja executada corretamente, aplicando os efeitos ou recompensas ao jogador conforme definido na implementação da função OnPickup. Se a colisão não for com o personagem do jogador, a função simplesmente retorna sem fazer nada, permitindo que outros tipos de colisões sejam ignorados.
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		OnPickup(Player); 

	}
}

// Função para resetar o estado do item, tornando-o ativo novamente e reposicionando-o se necessário. Isso pode ser útil para itens que reaparecem após serem coletados, ou para reiniciar o estado do item em um novo jogo ou nível. A implementação desta função depende do comportamento específico que você deseja para o item. Neste caso, a função simplesmente ativa o item, torna-o visível novamente e ativa a colisão para que o jogador possa apanhá-lo novamente. Dependendo do comportamento desejado, você pode adicionar lógica adicional para reposicionar o item ou aplicar outros efeitos ao resetar o item.
void ABasePickup::ResetPickup()
{
	bIsActive = true;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	
}





