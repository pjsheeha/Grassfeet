#include "GrassActor.h"

#include "Engine/Classes/Components/MeshComponent.h"
#include "Engine/Classes/Components/SkeletalMeshComponent.h"

// Sets default values
AGrassActor::AGrassActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGrassActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGrassActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GrassComponent->SetMorphTarget(L"Cube_000", 0.1f);
}

PointFillStatus AGrassActor::GetFillStatus()
{
	return this->FillStatus;
}

void AGrassActor::SetFillStatus(PointFillStatus Status)
{
	if (this->FillStatus == Status) return;

	switch (Status) {
	case PointFillStatus::Empty:
		GrassComponent->SetHiddenInGame(true);
		PregrassComponent->SetHiddenInGame(true);
		break;
	case PointFillStatus::Grass:
		GrassComponent->SetHiddenInGame(false);
		PregrassComponent->SetHiddenInGame(true);
		break;
	case PointFillStatus::Path:
		GrassComponent->SetHiddenInGame(true);
		PregrassComponent->SetHiddenInGame(false);
	}
	this->FillStatus = Status;
}

void AGrassActor::SetGrassMeshes(USkeletalMeshComponent *Grass, UMeshComponent *Pregrass) {
	this->GrassComponent = Grass;
	this->PregrassComponent = Pregrass;
}
