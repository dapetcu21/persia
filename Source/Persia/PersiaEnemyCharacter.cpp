#include "PersiaEnemyCharacter.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "RewindSnapshot.h"

const FName APersiaEnemyCharacter::NAME_PatrolDirection = TEXT("PatrolDirection");

void APersiaEnemyCharacter::SaveRewindSnapshot(struct FRewindActorSnapshot& Snapshot)
{
	Super::SaveRewindSnapshot(Snapshot);

	if (AAIController* AIController = Cast<AAIController>(Controller)) {
		if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent()) {
			Snapshot.PatrolDirection = BlackboardComponent->GetValueAsVector(NAME_PatrolDirection);
		}
	}
}

void APersiaEnemyCharacter::RestoreRewindSnapshot(const struct FRewindActorSnapshot& Snapshot)
{
	Super::RestoreRewindSnapshot(Snapshot);

	if (AAIController* AIController = Cast<AAIController>(Controller)) {
		if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent()) {
			BlackboardComponent->SetValueAsVector(NAME_PatrolDirection, Snapshot.PatrolDirection);
		}
	}
}
