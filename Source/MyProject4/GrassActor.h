#pragma once

#include "MapReaderActor.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	PointFillStatus GetFillStatus();
	void SetFillStatus(PointFillStatus Status);

private:
	UStaticMeshComponent *GrassComponent, *PregrassComponent;
	PointFillStatus FillStatus{ PointFillStatus::Empty };

	UFUNCTION(BlueprintCallable,
		meta = (DisplayName = "Set Grass Meshes",
			CompactNodeTitle = "SetGrassMeshes",
			Keywords = "grass mesh meshes"),
		Category = Game)
		void SetGrassMeshes(UStaticMeshComponent *Grass, UStaticMeshComponent *Pregrass);
};
