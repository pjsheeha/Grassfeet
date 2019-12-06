#pragma once

#include "MapReaderActor.h"

#include "CoreMinimal.h"
#include "Engine/Classes/Components/MeshComponent.h"
#include "Engine/Classes/Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Classes/Components/AudioComponent.h"
#include "GrassActor.generated.h"

class GrassAnimator
{
public:
	float Tick(float DeltaTime);
	void Footstep();

private:
	float GrassAnimTime = 0.0f;
	int32_t GrassAnimTick = -1;
};

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

	void Footstep();

private:
	USkeletalMeshComponent* GrassComponent;
	UMeshComponent* PregrassComponent;
	UAudioComponent* ac;
	PointFillStatus FillStatus{ PointFillStatus::Empty };
	GrassAnimator Animator;

	UFUNCTION(BlueprintCallable,
		meta = (DisplayName = "Set Grass Meshes",
			CompactNodeTitle = "SetGrassMeshes",
			Keywords = "grass mesh meshes"),
		Category = Game)
		void SetGrassMeshes(USkeletalMeshComponent *Grass, UMeshComponent *Pregrass, UAudioComponent *ac);
};
