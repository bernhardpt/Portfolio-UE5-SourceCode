#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ABaseProjectile.generated.h"

// Forward declarations para evitar incluir cabeçalhos desnecessários. As classes reais serão incluídas no .cpp
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class ZOMBIESHOOTER_API AABaseProjectile : public AActor
{
	// Macro necessário para a reflexão do Unreal Engine, que permite que a classe seja usada no editor, em Blueprints, e para outras funcionalidades do UE. O GENERATED_BODY() é obrigatório em todas as classes que herdam de UObject ou AActor, e deve ser colocado no início da definição da classe.
	GENERATED_BODY()
	
public:
	// Construtor padrão para configurar os componentes e valores iniciais do projétil. O construtor é chamado quando o ator é criado, e é onde você deve configurar os componentes (como a mesh, colisão, movimento) e definir valores padrão para as variáveis.
	AABaseProjectile();

protected:
	// Função chamada quando o jogo começa ou quando o ator é spawnado. Aqui você pode colocar qualquer lógica de inicialização que precise ser feita depois que o ator já foi criado e todos os componentes estão configurados.
	virtual void BeginPlay() override;

public:	
	// Função chamada a cada frame. O DeltaTime é o tempo em segundos desde o último frame, e pode ser usado para fazer cálculos de movimento ou outras lógicas que precisam ser atualizadas constantemente.
	virtual void Tick(float DeltaTime) override;
	
	// Componentes do projétil. O CollisionComp é um componente de colisão que detecta quando o projétil colide com algo. O ProjectileMovement é um componente que controla o movimento do projétil, como velocidade e direção. O ProjectileMesh é a representação visual do projétil. O DamageValue é o valor de dano que o projétil causará ao atingir um alvo.
	UPROPERTY(VisibleDefaultsOnly, Category = "C++ | Projétil")
	USphereComponent* CollisionComp;

	// O componente de movimento do projétil é responsável por mover o projétil na direção correta com a velocidade definida. Ele deve ser configurado para usar a velocidade inicial e máxima, e para se mover na direção da rotação do projétil.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Movimento")
	UProjectileMovementComponent* ProjectileMovement;

	// O componente de mesh do projétil é a representação visual do projétil. Ele deve ser configurado para usar uma mesh adequada (por exemplo, uma esfera ou um modelo de bala) e para ter colisão desativada, já que a colisão é tratada pelo CollisionComp.
	UPROPERTY(VisibleAnywhere, Category = "C++ | Projétil")
	UStaticMeshComponent* ProjectileMesh;

	// O valor de dano do projétil é o quanto de dano ele causará ao atingir um alvo. Esse valor pode ser configurado no editor para cada tipo de projétil, e pode ser usado na função OnHit para aplicar dano ao ator atingido.
	float DamageValue = 10.0f;

	// Função que é chamada quando o projétil colide com algo. Essa função deve ser vinculada ao evento de colisão do CollisionComp, para que seja chamada automaticamente quando o projétil colidir com outro ator. A função recebe informações sobre a colisão, como o componente que foi atingido, o ator atingido, a localização e normal da colisão, etc. Aqui você pode implementar a lógica para aplicar dano ao ator atingido, spawnar efeitos visuais ou sonoros, e destruir o projétil.
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
};




