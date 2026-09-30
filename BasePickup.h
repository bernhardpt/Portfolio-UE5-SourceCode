// Preencher aviso de copyright no editor do Unreal.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "BasePickup.generated.h"

// Forward declarations para evitar incluir cabeçalhos desnecessários. Isso ajuda a reduzir o tempo de compilação e a manter as dependências mais leves. As classes são usadas como ponteiros ou referências no código, então não precisamos do cabeçalho completo aqui.
class USphereComponent;
class URotatingMovementComponent;
class APlayerCharacter;

UCLASS()
class ZOMBIESHOOTER_API ABasePickup : public AActor
{
	// Macro necessário para habilitar as funcionalidades de reflexão e outras características do Unreal Engine. Ele deve ser colocado dentro da definição da classe, geralmente logo após a declaração da classe e antes dos membros públicos ou privados.
	GENERATED_BODY()
	
public:	
	// Construtor padrão para configurar os componentes e valores iniciais da classe. Ele é chamado quando o ator é criado, seja no editor ou durante o jogo. Aqui é onde você deve criar os componentes (como a mesh, colisores, etc.) e configurar suas propriedades básicas.
	ABasePickup();

protected:
	// Função chamada quando o jogo começa ou quando o ator é spawnado. É um bom lugar para inicializar variáveis, configurar estados iniciais, ou realizar qualquer configuração que dependa do mundo ou de outros atores já estarem disponíveis. Ele é chamado apenas uma vez durante a vida do ator.
	virtual void BeginPlay() override;

public:	
	// Função chamada a cada frame. O DeltaTime é o tempo em segundos desde o último frame, o que é útil para fazer cálculos de movimento ou outras ações que dependem do tempo. Ele é chamado continuamente enquanto o ator estiver ativo no mundo.
	virtual void Tick(float DeltaTime) override;

	// Função para lidar com a lógica de quando o item é coletado pelo jogador. Ela recebe um ponteiro para o personagem do jogador que coletou o item, o que permite aplicar efeitos ou dar recompensas ao jogador. Esta função deve ser implementada para definir o comportamento específico do item quando ele é coletado.
	UPROPERTY(VisibleAnywhere, Category = "C++ | Componentes")
	UStaticMeshComponent* MeshComp;

	// Componentes para a lógica de coleta e rotação do item. O SphereComp é usado para detectar quando o jogador entra na área de coleta, e o RotatingComp é usado para fazer o item girar lentamente, o que é um efeito visual comum para itens coletáveis.
	UPROPERTY(VisibleAnywhere, Category = "C++ | Componentes")
	USphereComponent* SphereComp;

	// O componente de movimento rotativo é usado para fazer o item girar lentamente, o que é um efeito visual comum para itens coletáveis. Ele pode ser configurado para controlar a velocidade e o eixo de rotação, e é uma maneira fácil de adicionar um pouco de vida ao item no mundo.
	UPROPERTY(VisibleAnywhere, Category = "C++ | Componentes")
	URotatingMovementComponent* RotatingComp;

	// Função para lidar com a lógica de quando o item é coletado pelo jogador. Ela recebe um ponteiro para o personagem do jogador que coletou o item, o que permite aplicar efeitos ou dar recompensas ao jogador. Esta função deve ser implementada para definir o comportamento específico do item quando ele é coletado.
	virtual void OnPickup(class APlayerCharacter* Player);

	// Função para lidar com a lógica de quando o item é coletado pelo jogador. Ela é chamada automaticamente pelo sistema de colisão do Unreal quando outro ator entra na área de colisão definida pelo SphereComp. O OtherActor é o ator que colidiu com o SphereComp, e podemos verificar se é o personagem do jogador para determinar se devemos chamar a função OnPickup.
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	// Função para resetar o estado do item, tornando-o ativo novamente e reposicionando-o se necessário. Isso pode ser útil para itens que reaparecem após serem coletados, ou para reiniciar o estado do item em um novo jogo ou nível. A implementação desta função depende do comportamento específico que você deseja para o item.
	void ResetPickup();

	// Variável para controlar se o item está ativo ou não. Quando o item é coletado, ele pode ser desativado para impedir que seja coletado novamente até que seja resetado. Isso é útil para itens que devem desaparecer temporariamente após serem coletados, ou para controlar a disponibilidade do item no mundo.
	bool bIsActive = true;

	// Variável para armazenar o som que é tocado quando o item é coletado. Isso pode ser configurado no editor para cada tipo de item, permitindo que diferentes itens tenham sons de coleta distintos. O som deve ser configurado para ser curto e apropriado para um efeito de coleta, para melhorar a experiência do jogador.
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Som")
	USoundBase* PickupSound;
	
};




