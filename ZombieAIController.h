#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "ZombieAIController.generated.h"

class UBehaviorTree;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

UCLASS()
class ZOMBIESHOOTER_API AZombieAIController : public AAIController
{
	GENERATED_BODY()

public:
	AZombieAIController();
	
	// Behavior Tree asset assigned in editor
	UPROPERTY(EditDefaultsOnly, Category = "C++ | AI")
	UBehaviorTree* BehaviorTreeAsset;

	// Perception component managing senses
	UPROPERTY(VisibleAnywhere, Category = "C++ | AI")
	UAIPerceptionComponent* AIPerceptionComp;

	UAISenseConfig_Sight* SightConfig;
	UAISenseConfig_Hearing* HearingConfig;

	// Callback triggered when perception stimuli are updated
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);

	virtual void OnPossess(APawn* InPawn) override;
};