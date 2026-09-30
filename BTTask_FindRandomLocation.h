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
	// Core execution function triggered by the Behavior Tree
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	// Maximum radius to search for a valid patrol point on the NavMesh
	UPROPERTY(EditAnywhere, Category = "C++ | AI")
	float SearchRadius = 500.0f;
};