#pragma once

#include "array.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsPublic.h"
#include "PhysXIncludes.h"
#include "geometry/PxTriangleMesh.h"
#include "foundation/PxSimpleTypes.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include <vector>

#include "MapReaderActor.generated.h"

enum class PointFillStatus : uint8 {
	Empty,
	Path,
	Grass
};
struct FPoint {
	PointFillStatus fill_status;
	bool has_cow;
	FTransform transform{};
	uint32_t group{};

	std::vector<uint32_t> next;

	FPoint() : fill_status(PointFillStatus::Empty), has_cow() {}

	bool isGrass()
	{
		return fill_status == PointFillStatus::Grass || fill_status == PointFillStatus::Path;
	}
};

UCLASS()
class MYPROJECT4_API AMapReaderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapReaderActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	std::vector<FPoint>& GetMap();
	std::vector<FTransform>& GetGroups();

private:
	std::vector<FPoint> Map{};
	std::vector<FTransform> Groups{};

	void InitializeMap();
};
