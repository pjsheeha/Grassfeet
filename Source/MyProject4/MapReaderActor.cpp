#include "MapReaderActor.h"

#include "Util.h"

#include "Containers/array.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsPublic.h"
#include "PhysXIncludes.h"
#include "geometry/PxTriangleMesh.h"
#include "foundation/PxSimpleTypes.h"
#include "Engine/StaticMesh.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/Paths.h"
#include "StaticMeshResources.h"

#include <cstdio>
#include <string>
#include <vector>


// 1. Because we use graph to represent data, we have to ensure the player
//    never go from one point to a non-neighboring point. And grass can only be
//    filled by neighboring points.


// Sets default values
AMapReaderActor::AMapReaderActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMapReaderActor::BeginPlay()
{
	Super::BeginPlay();
	this->InitializeMap();
}

// Called every frame
void AMapReaderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool read(FILE* f, std::vector<FPoint>& array) {
	array = {};

	uint8_t buffer[16];

	if (fread(buffer, 10, 1, f) != 1) return false;
	if (buffer[0] != 'G' || buffer[1] != 'R' || buffer[2] != 'A' || buffer[3] != 'S'
		|| buffer[4] != 'S' || buffer[5] != 'F' || buffer[6] != 'E' || buffer[7] != 'E'
		|| buffer[8] != 'T') {
		return false;
	}
	// Version
	if (buffer[9] != 1) return false;

	if (fread(buffer, 4, 1, f) != 1) return false;
	uint32_t vertices = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + (buffer[3]);

	for (uint32_t i = 0; i < vertices; i++) {
		float position[3];
		for (int j = 0; j < 3; j++) {
			if (fread(buffer, 4, 1, f) != 1) return false;
			uint32_t position32 = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + (buffer[3]);
			position[j] = *reinterpret_cast<float*>(&position32);
		}

		float normal[3];
		for (int j = 0; j < 3; j++) {
			if (fread(buffer, 4, 1, f) != 1) return false;
			uint32_t normal32 = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + (buffer[3]);
			normal[j] = *reinterpret_cast<float*>(&normal32);
		}

		if (fread(buffer, 4, 1, f) != 1) return false;
		uint32_t edges = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + (buffer[3]);

		std::vector<uint32_t> next;
		next.reserve(edges);

		for (uint32_t j = 0; j < edges; j++) {
			if (fread(buffer, 4, 1, f) != 1) return false;
			uint32_t v = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + (buffer[3]);
			next.push_back(v);
		}

		FVector position_v(position[0], position[1], position[2]);
		FVector normal_v(normal[0], normal[1], normal[2]);

		FPoint point;
		point.next = std::move(next);
		point.transform.SetLocation(position_v);
		point.transform.SetRotation(FQuat(UKismetMathLibrary::MakeRotFromZ(normal_v)));
		array.push_back(point);
	}

	return true;
}

void AMapReaderActor::InitializeMap() {
	FString dir = FPaths::ProjectDir();
	std::string file = std::string(TCHAR_TO_UTF8(*dir)) + "/map-sphere.gra";
	FILE* f = fopen(file.c_str(), "rb");

	if (!f) {
		GF_LOG(L"Cannot read map-sphere.gra");
		this->Map = {};
	}

	std::vector<FPoint> array;
	bool res = read(f, array);

	fclose(f);

	if (res)
		this->Map = array;
	else
		this->Map = {};
}

std::vector<FPoint>& AMapReaderActor::GetMap() {
	return this->Map;
}
