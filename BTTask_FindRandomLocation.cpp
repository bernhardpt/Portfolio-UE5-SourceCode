#include "BTTask_FindRandomLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation()
{
	// Set node name for easier identification in the Behavior Tree editor
	NodeName = TEXT("Find Random Location"); 
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Retrieve AI Controller and controlled Pawn
	AAIController* AIController = OwnerComp.GetAIOwner(); 
	APawn* AIPawn = AIController ? AIController->GetPawn() : nullptr; 

	if (!AIPawn)
	{
		return EBTNodeResult::Failed; 
	}

	// Use the pawn's current location as the search origin
	FVector Origin = AIPawn->GetActorLocation(); 

	// Access the Navigation System to find valid NavMesh bounds
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()); 

	if (!NavSys)
	{
		return EBTNodeResult::Failed; 
	}

	FNavLocation RandomPoint; 
	
	// Attempt to find a reachable point within the specified radius
	if (NavSys->GetRandomReachablePointInRadius(Origin, SearchRadius, RandomPoint))
	{
		// Store the valid location in the assigned Blackboard key
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), RandomPoint.Location); 
        
		return EBTNodeResult::Succeeded; 
	}

	return EBTNodeResult::Failed; 
}