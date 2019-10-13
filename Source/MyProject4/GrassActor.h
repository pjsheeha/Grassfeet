#pragma once

#include "Util.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include <vector>

#include "GrassActor.generated.h"


UCLASS()
class MYPROJECT4_API AGrassActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrassActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	
};
