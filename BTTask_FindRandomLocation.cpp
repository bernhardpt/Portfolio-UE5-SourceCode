// Preencher aviso de copyright no editor do Unreal.


#include "BTTask_FindRandomLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation()
{
	NodeName = TEXT("Find Random Location"); // Define o nome do nó para facilitar a identificação na árvore de comportamento

}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner(); // Obtém o controlador de IA que está executando esta tarefa

	APawn* AIPawn = AIController ? AIController->GetPawn() : nullptr; // Obtém o personagem controlado pela IA. Se o controlador for nulo, AIPawn será definido como nullptr.

	if (!AIPawn)
	{
		return EBTNodeResult::Failed; // Se o personagem controlado for nulo, a tarefa falha, pois não há um local para encontrar um ponto aleatório

	}

	FVector Origin = AIPawn->GetActorLocation(); // Obtém a localização atual do personagem controlado pela IA, que será usada como ponto de origem para encontrar um local aleatório

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()); // Obtém o sistema de navegação atual do mundo, que é necessário para encontrar um ponto aleatório navegável. Se o sistema de navegação for nulo, a tarefa falha, pois não é possível encontrar um local aleatório sem ele.

	if (!NavSys)
	{
		return EBTNodeResult::Failed; // Se o sistema de navegação for nulo, a tarefa falha, pois não é possível encontrar um local aleatório sem ele.

	}

	FNavLocation RandomPoint; // Variável para armazenar o local aleatório encontrado. Se a função GetRandomReachablePointInRadius retornar true, RandomPoint conterá um local aleatório navegável dentro do raio especificado.
	
	if (NavSys->GetRandomReachablePointInRadius(Origin, SearchRadius, RandomPoint))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), RandomPoint.Location); // Se um local aleatório navegável for encontrado, ele é armazenado no componente de
        
		return EBTNodeResult::Succeeded; // A tarefa é bem-sucedida, indicando que um local aleatório foi encontrado e armazenado no Blackboard. O local aleatório é definido como o valor do vetor associado à chave selecionada no Blackboard.

	}

	return EBTNodeResult::Failed; // Se a função GetRandomReachablePointInRadius retornar false, significa que não foi possível encontrar um local aleatório navegável dentro do raio especificado, e a tarefa falha.
}




