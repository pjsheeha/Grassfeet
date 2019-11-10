#pragma once

#include "MapReaderActor.h"

#include "CoreMinimal.h"
#include "Engine/Classes/Components/MeshComponent.h"
#include "Engine/Classes/Components/SkeletalMeshComponent.h"
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
	USkeletalMeshComponent* GrassComponent;
	UMeshComponent* PregrassComponent;
	PointFillStatus FillStatus{ PointFillStatus::Empty };

	UFUNCTION(BlueprintCallable,
		meta = (DisplayName = "Set Grass Meshes",
			CompactNodeTitle = "SetGrassMeshes",
			Keywords = "grass mesh meshes"),
		Category = Game)
		void SetGrassMeshes(USkeletalMeshComponent *Grass, UMeshComponent *Pregrass);
};
