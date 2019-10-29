#include "GrassActor.h"

// Sets default values
AGrassActor::AGrassActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
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
	// Note: Tick is turned off!
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

void AGrassActor::SetGrassMeshes(UMeshComponent *Grass, UMeshComponent *Pregrass) {
	this->GrassComponent = Grass;
	this->PregrassComponent = Pregrass;
}
