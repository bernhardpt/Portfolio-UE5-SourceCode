// Preencher aviso de copyright no editor do Unreal.


#include "BTTask_Attack.h"
#include "AIController.h"
#include "ZombieCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack Player"); // Define o nome do nó para facilitar a identificação na árvore de comportamento
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner(); // Obtém o controlador de IA que está executando esta tarefa

	if (AZombieCharacter* Zombie = Cast<AZombieCharacter>(AIController->GetPawn()))
	{
		Zombie->PlayAttackAnimation(); // Chama a função para reproduzir a animação de ataque no personagem zumbi
		return EBTNodeResult::Succeeded; // Retorna sucesso para indicar que a tarefa foi concluída com êxito

	}

	return EBTNodeResult::Failed; // Retorna falha para indicar que a tarefa não foi concluída com êxito

}
