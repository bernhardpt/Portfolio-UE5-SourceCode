// Preencher aviso de copyright no editor do Unreal.


#include "ABaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AABaseProjectile::AABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Cria o componente de colisão do projétil, que é uma esfera. O CollisionComp é usado para detectar quando o projétil colide com algo. Ele é configurado para ter um raio de 5 unidades, usar o perfil de colisão "Projectile", e chamar a função OnHit quando ocorrer uma colisão. O CollisionComp é definido como o RootComponent do ator, o que significa que ele é a base para a hierarquia de componentes do ator.
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f); 
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile"); 
	CollisionComp->OnComponentHit.AddDynamic(this, &AABaseProjectile::OnHit); 
	RootComponent = CollisionComp;
	
	// Cria o componente de mesh do projétil, que é a representação visual do projétil. Ele é configurado para usar o perfil de colisão "NoCollision", já que a colisão é tratada pelo CollisionComp. O ProjectileMesh é anexado ao CollisionComp, o que significa que ele seguirá a posição e rotação do CollisionComp. O ProjectileMesh deve ser configurado para usar uma mesh adequada (por exemplo, uma esfera ou um modelo de bala) no editor, para que o projétil tenha uma aparência visual.
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(CollisionComp);
	ProjectileMesh->SetCollisionProfileName("NoCollision");
	
	// Cria o componente de movimento do projétil, que é responsável por mover o projétil na direção correta com a velocidade definida. Ele é configurado para usar o CollisionComp como o componente atualizado, o que significa que ele moverá o CollisionComp (e, por extensão, o ProjectileMesh) de acordo com a velocidade e direção definidas. O InitialSpeed e MaxSpeed são definidos como 0, o que significa que eles devem ser configurados no editor ou em tempo de execução para que o projétil se mova. O bRotationFollowsVelocity é definido como true, o que significa que a rotação do projétil seguirá a direção do movimento. O bShouldBounce é definido como false, o que significa que o projétil não irá quicar quando colidir com algo. O InitialLifeSpan é definido como 3 segundos, o que significa que o projétil será destruído automaticamente após 3 segundos se não colidir com nada. O ProjectileGravityScale é definido como 0, o que significa que o projétil não será afetado pela gravidade.
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 0.f;
	ProjectileMovement->MaxSpeed = 0.f; 
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	InitialLifeSpan = 3.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	
}

void AABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// A função OnHit é chamada automaticamente quando o projétil colide com outro ator, graças à ligação feita no construtor com CollisionComp->OnComponentHit.AddDynamic(this, &AABaseProjectile::OnHit). A função recebe informações sobre a colisão, como o componente que foi atingido (HitComp), o ator atingido (OtherActor), o componente do ator atingido (OtherComp), a força da colisão (NormalImpulse) e detalhes adicionais sobre a colisão (Hit). Dentro dessa função, você pode implementar a lógica para aplicar dano ao ator atingido usando UGameplayStatics::ApplyDamage, spawnar efeitos visuais ou sonoros, e destruir o projétil usando Destroy(). A lógica dentro do if verifica se o OtherActor é válido, se não é o próprio projétil, e se não é o ator que disparou o projétil (GetOwner()), para garantir que o projétil não cause dano a si mesmo ou ao ator que o disparou.
void AABaseProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{

	// Verifica se o OtherActor é válido, se não é o próprio projétil, e se não é o ator que disparou o projétil (GetOwner()). Isso é importante para garantir que o projétil não cause dano a si mesmo ou ao ator que o disparou. Se essas condições forem verdadeiras, a função aplica dano ao OtherActor usando UGameplayStatics::ApplyDamage, passando o valor de dano (DamageValue), o controlador do instigador (GetInstigatorController()), o próprio projétil como a fonte do dano (this), e o tipo de dano (UDamageType::StaticClass()). Depois de aplicar o dano, a função chama Destroy() para destruir o projétil, já que ele atingiu um alvo e não deve mais existir no mundo do jogo.
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherActor != GetOwner()))
	{
		UGameplayStatics::ApplyDamage(OtherActor, DamageValue, GetInstigatorController(), this, UDamageType::StaticClass()); 
		Destroy();
		
	}
	
}





