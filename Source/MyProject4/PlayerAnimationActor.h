#pragma once

#include "CoreMinimal.h"

#include "Animation/AnimationAsset.h" 
#include "Engine/Classes/Components/MeshComponent.h"
#include "Engine/Classes/Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "PlayerAnimationActor.generated.h"

UCLASS()
class MYPROJECT4_API APlayerAnimationActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerAnimationActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable,
		meta = (DisplayName = "Initialize",
			CompactNodeTitle = "Initialize",
			Keywords = "initialize"),
		Category = Game)
		void Initialize(USkeletalMeshComponent* Mesh, UAnimationAsset *Idle, UAnimationAsset *Walk);
	
	UFUNCTION(BlueprintCallable,
		meta = (DisplayName = "Update Mesh",
			CompactNodeTitle = "UpdateMesh",
			Keywords = "player update mesh"),
		Category = Game)
		void UpdateMesh(FVector2D MoveDirection);

private:
	USkeletalMeshComponent* PlayerMesh{};
	UAnimationAsset* IdleAnim{}, * WalkAnim{};
	bool Walking{ false };
};
