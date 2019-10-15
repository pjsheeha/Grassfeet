#include "GrassSpawnActor.h"

#include "generate_graph.h"
#include "GrassActor.h"
#include "Util.h"

#include "EngineUtils.h"
#include "foundation/PxSimpleTypes.h"
#include "GameFramework/Controller.h"
#include "geometry/PxTriangleMesh.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsPublic.h"
#include "PhysXIncludes.h"
#include "Runtime/Engine/Public/PhysXPublic.h"

#include <memory>

// Sets default values
AGrassSpawnActor::AGrassSpawnActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGrassSpawnActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGrassSpawnActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGrassSpawnActor::UpdateGrass(UPARAM(ref) TArray<FPoint> &Points, AActor *Planet, UStaticMeshComponent *MeshComponent)
{
	if (!Planet || !MeshComponent || !this->GrassActorClass) return;

	auto world = GetWorld();
	if (!world) return;

	if (this->GrassActors.empty()) {
		TArray<UStaticMeshComponent*> planet_mesh_components;
		Planet->GetComponents<UStaticMeshComponent>(planet_mesh_components);
		if (planet_mesh_components.Num() <= 0) return;
		auto planet_body_setup = planet_mesh_components[0]->BodyInstance.BodySetup.Get();
		if (!planet_body_setup) return;

		auto setup = MeshComponent->BodyInstance.BodySetup.Get();
		if (!setup) return;
		physx::PxTriangleMesh* triangles = setup->TriMeshes[0];
		auto triangle_array = triangles->getTriangles();

		this->GrassActors.resize(Points.Num());
		for (size_t i = 0; i < Points.Num(); i++) {
			/*
			// Get the local location of the vertices of the point's triangle.
			FVector x, y, z;
			if (triangles->getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES)
			{
				auto indices = (reinterpret_cast<const PxU16*>(triangle_array));
				x = P2UVector(triangles->getVertices()[indices[i * 3 + 0]]);
				y = P2UVector(triangles->getVertices()[indices[i * 3 + 1]]);
				z = P2UVector(triangles->getVertices()[indices[i * 3 + 2]]);
			}
			else {
				auto indices = (reinterpret_cast<const PxU32*>(triangle_array));
				x = P2UVector(triangles->getVertices()[indices[i * 3 + 0]]);
				y = P2UVector(triangles->getVertices()[indices[i * 3 + 1]]);
				z = P2UVector(triangles->getVertices()[indices[i * 3 + 2]]);
			}

			// Get the local location of the point.
			FVector location = (x + y + z) / 3;

			// Transform point location to world.
			location = Planet->ActorToWorld().TransformPosition(location);

			// Get grass point and normal.
			FVector closest, normal;
			planet_body_setup->GetClosestPointAndNormal(location, Planet->ActorToWorld(), closest, normal);

			// Build grass transform.
			FTransform spawn_transform{};
			spawn_transform.SetLocation(closest);
			spawn_transform.SetRotation(FQuat(UKismetMathLibrary::MakeRotFromZ(normal)));
			*/

			// Spawn grass.
			AGrassActor* grass = world->SpawnActor<AGrassActor>(this->GrassActorClass, Points[i].transform * Planet->ActorToWorld());

			this->GrassActors[i] = grass;
		}
	}

	for (size_t i = 0; i < Points.Num(); i++) {
		if (this->GrassActors[i]->GetFillStatus() != Points[i].fill_status) {
			GF_LOG(L"GrassSpawnActor: Setting point %d to fill %d", i, Points[i].fill_status);
			this->GrassActors[i]->SetFillStatus(Points[i].fill_status);
		}
	}
}

void AGrassSpawnActor::SetGrassActorClass(UClass *Class)
{
	this->GrassActorClass = Class;
}
