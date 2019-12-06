#include "GrassActor.h"

#include "Engine/Classes/Components/MeshComponent.h"
#include "Engine/Classes/Components/SkeletalMeshComponent.h"

#include <random>

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

	float target = this->Animator.Tick(DeltaTime);
	if (target < 0.2f) {
		target = 0.2f;
	}
	GrassComponent->SetMorphTarget(L"Cube_000", target);
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
		arrivalsound->Play();
		break;
	case PointFillStatus::Path:
		GrassComponent->SetHiddenInGame(true);
		PregrassComponent->SetHiddenInGame(false);
		PregrassComponent->SetWorldScale3D(FVector::OneVector * 2);
		pregrasssound->Play();
	}
	this->FillStatus = Status;
}

void AGrassActor::Footstep()
{
	this->Animator.Footstep();
}

void AGrassActor::SetGrassMeshes(USkeletalMeshComponent *Grass, UMeshComponent *Pregrass, UAudioComponent *postgrasssound, UAudioComponent *pregrasssound, UAudioComponent *arrivalsound) {
	this->GrassComponent = Grass;
	this->PregrassComponent = Pregrass;
	this->pregrasssound = pregrasssound;
	this->arrivalsound = arrivalsound;
	this->Animator.postgrasssound = postgrasssound;
}

float GrassAnimator::Tick(float DeltaTime) {
	if (GrassAnimTick == -1) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint32_t> tick(0, 179);
		GrassAnimTick = tick(gen);
	}

	GrassAnimTime += DeltaTime;
	if (GrassAnimTime > 1024.0f) {
		GrassAnimTime = 1024.0f;
	}

	uint32_t count = GrassAnimTime * 120.f;
	GrassAnimTime -= count * (1.0f / 120.0f);
	if (GrassAnimTime < 0.0f) GrassAnimTime = 0.0f;

	if (GrassAnimTick < 180) {
		GrassAnimTick += count;
		GrassAnimTick %= 180;

		if (GrassAnimTick >= 140) {
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<uint32_t> tick(0, 24);
			if (tick(gen) < 1) {
				GrassAnimTick = 180 - GrassAnimTick;
			}
		}

		if (GrassAnimTick < 90) {
			return 0.5f + 0.5f / 90.0f * GrassAnimTick;
		}
		else {
			return 1.0f - 0.5f / 90.0f * (GrassAnimTick - 90);
		}
	}
	else {
		GrassAnimTick += count * 6;
		if (GrassAnimTick < 360) {
			return 1.0f - (GrassAnimTick - 180) / 180.0f;
		}
		else if (GrassAnimTick < 450) {
			return (GrassAnimTick - 360) / 180.0f;
		}
		else {
			GrassAnimTick = 0;
			postgrasssound->Play();
			return 0.5f;
		}
	}
}

void GrassAnimator::Footstep() {
	if (GrassAnimTick < 90) {
		GrassAnimTick = 270 - GrassAnimTick;
	}
	else if (GrassAnimTick < 180) {
		GrassAnimTick += 90;
	}
	else if (GrassAnimTick >= 360) {
		GrassAnimTick = 270 + (450 - GrassAnimTick);
	}
}
