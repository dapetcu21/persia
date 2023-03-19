#include "PersiaPlayerController.h"

#include "Kismet/GameplayStatics.h"

void APersiaPlayerController::RestartGame_Implementation()
{
	UGameplayStatics::OpenLevel(this, TEXT("ThirdPersonMap"));
}
