// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "array.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsPublic.h"
#include "PhysXIncludes.h"
#include "geometry/PxTriangleMesh.h"
#include "foundation/PxSimpleTypes.h"
#include "generate_graph.generated.h"

UENUM(BlueprintType)
enum class PointFillStatus : uint8 {
	Empty,
	Path,
	Grass
};
USTRUCT(BlueprintType)
struct FPoint {

	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flower Struct")
    PointFillStatus fill_status;
	bool has_cow;
	TArray<PxU32> next;
	FPoint() : fill_status(PointFillStatus::Empty), has_cow() {}

	bool isGrass()
	{
		return fill_status == PointFillStatus::Grass || fill_status == PointFillStatus::Path;
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT4_API Ugenerate_graph : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	// Sets default values for this component's properties
	Ugenerate_graph();
	UFUNCTION(BlueprintPure,
		meta = (DisplayName = "Makes a Graph",
			CompactNodeTitle = "make_graph",
			Keywords = "graph make grassfeet"),
		Category = Game)
		static TArray<FPoint> make_graph(UStaticMeshComponent *mesh);
	UFUNCTION(BlueprintPure,
		meta = (DisplayName = "Is it grass?",
			CompactNodeTitle = "is_graph",
			Keywords = "grass is grassfeet"),
		Category = Game)
		static bool IsGrass(TArray<FPoint> graph, int index);



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
