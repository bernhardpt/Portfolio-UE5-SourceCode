// Preencher aviso de copyright no editor do Unreal.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindRandomLocation.generated.h"

UCLASS()
class ZOMBIESHOOTER_API UBTTask_FindRandomLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_FindRandomLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override; //Função que é chamada quando a tarefa é executada

private:
	UPROPERTY(EditAnywhere, Category = "C++ | AI")
	float SearchRadius = 500.0f; //Raio de busca para encontrar um ponto aleatório

};
