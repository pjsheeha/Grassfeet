#include "PlayerAnimationActor.h"

#include "Animation/AnimationAsset.h" 
#include "Engine/Classes/Components/MeshComponent.h"
#include "Engine/Classes/Components/SkeletalMeshComponent.h"

// Sets default values
APlayerAnimationActor::APlayerAnimationActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APlayerAnimationActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerAnimationActor::Tick(float DeltaTime)
{
	// Tick disabled
	Super::Tick(DeltaTime);

}

void APlayerAnimationActor::Initialize(USkeletalMeshComponent* Mesh, UAnimationAsset* Idle, UAnimationAsset* Walk)
{
	this->PlayerMesh = Mesh;
	this->IdleAnim = Idle;
	this->WalkAnim = Walk;
	this->Walking = false;
}

void APlayerAnimationActor::UpdateMesh(FVector2D MoveDirection)
{
	if (!this->PlayerMesh || !this->IdleAnim || !this->WalkAnim) return;

	if (MoveDirection.X * MoveDirection.X + MoveDirection.Y * MoveDirection.Y < 0.1f) {
		// Idle
		if (this->Walking) {
			this->Walking = false;
			this->PlayerMesh->PlayAnimation(this->IdleAnim, true);
		}
	}
	else {
		// Walking
		if (!this->Walking) {
			this->Walking = true;
			this->PlayerMesh->PlayAnimation(this->WalkAnim, true);
		}
	}
}
