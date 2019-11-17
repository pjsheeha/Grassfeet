#include "GrassSpawnActor.h"

#include "GrassActor.h"
#include "MapReaderActor.h"
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

#include <algorithm>
#include <cmath>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

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

enum Closeness {
	Close,
	Near,
	Far
};

std::pair<Closeness, float> GetCloseness(FTransform& Transform, AActor* Planet, const TArray<AActor*>& Players, bool isPath) {
	float min_dist = std::numeric_limits<float>::max();
	for (auto& player : Players) {
		float dist = FVector::DistSquared((Transform * Planet->ActorToWorld()).GetLocation(), player->GetActorLocation());
		min_dist = std::min(min_dist, dist);
	}
	Closeness closeness = Closeness::Far;
	if (min_dist < (isPath ? 50000: 160000)) {
		closeness = Closeness::Close;
	}
	else if (min_dist < 400000) {
		closeness = Closeness::Near;
	}
	return { closeness, min_dist };
}

void AGrassSpawnActor::UpdateGrass(AMapReaderActor* MapReader, AActor* Planet, UMeshComponent* MeshComponent, TArray<AActor*> Players)
{
	if (!Planet || !MeshComponent || !this->GrassActorClass) return;
	this->UpdatePathActors(MapReader, Planet, MeshComponent, Players);
	this->UpdateGrassActors(MapReader, Planet, MeshComponent, Players);
	this->ClearFootsteps(MapReader);
}

void AGrassSpawnActor::UpdatePathActors(AMapReaderActor* MapReader, AActor* Planet, UMeshComponent* MeshComponent, TArray<AActor*> Players)
{
	auto world = GetWorld();
	if (!world) return;

	auto& points = MapReader->GetMap();
	if (points.size() == 0) return;

	std::vector<std::pair<uint32_t, float>> actors_to_spawn;
	std::unordered_map<uint32_t, AGrassActor*> new_actors;

	// Prepare to spawn new actors and update old actors
	for (uint32_t i = 0; i < points.size(); i++) {
		if (points[i].fill_status == PointFillStatus::Path) {
			Closeness closeness;
			float dist;
			std::tie(closeness, dist) = GetCloseness(points[i].transform, Planet, Players, true);

			float x = fmod(points[i].transform.GetLocation().X, 2.0f);
			float y = fmod(points[i].transform.GetLocation().Y, 2.0f);
			float z = fmod(points[i].transform.GetLocation().Z, 2.0f);
			if (x < 0) x += 4;
			if (y < 0) y += 4;
			if (z < 0) z += 4;
			uint32_t sum = static_cast<uint32_t>(x) + static_cast<uint32_t>(y) + static_cast<uint32_t>(z);
			bool even = (sum % 2 == 0);

			if (closeness == Closeness::Close || (closeness == Closeness::Near && even)) {
				auto old_actor_it = this->PathActors.find(i);
				if (old_actor_it != this->PathActors.end()) {
					// Existing
					old_actor_it->second->SetFillStatus(PointFillStatus::Path);
					new_actors[i] = old_actor_it->second;
				}
				else {
					// New
					actors_to_spawn.push_back(std::make_pair(i, dist));
				}
			}
		}
	}

	// Sort actors to spawn
	std::sort(actors_to_spawn.begin(), actors_to_spawn.end(), [](const auto& a, const auto& b) { return a.second < b.second; });

	// Remove unused actors
	for (auto& actor : this->PathActors) {
		if (new_actors.find(actor.first) == new_actors.end()) {
			actor.second->Destroy();
		}
	}

	// Spawn a limited number of new actors
	for (int i = 0; i < actors_to_spawn.size() && i < 100; i++) {
		AGrassActor* actor = world->SpawnActor<AGrassActor>(this->GrassActorClass, points[actors_to_spawn[i].first].transform * Planet->ActorToWorld());
		actor->SetFillStatus(PointFillStatus::Path);
		new_actors[actors_to_spawn[i].first] = actor;
	}
	this->PathActors = std::move(new_actors);
}

void AGrassSpawnActor::UpdateGrassActors(AMapReaderActor* MapReader, AActor* Planet, UMeshComponent* MeshComponent, TArray<AActor*> Players)
{
	auto world = GetWorld();
	if (!world) return;

	auto& group_transforms= MapReader->GetGroups();

	std::unordered_set<uint32_t> grass_groups;
	std::unordered_set<uint32_t> footstep_groups;
	{
		auto& points = MapReader->GetMap();
		if (points.size() == 0) return;
		for (uint32_t i = 0; i < points.size(); i++) {
			if (points[i].fill_status == PointFillStatus::Grass) {
				grass_groups.insert(points[i].group);
				if (points[i].footstep) footstep_groups.insert(points[i].group);
			}
		}
	}

	std::vector<std::pair<uint32_t, float>> actors_to_spawn;
	std::unordered_map<uint32_t, AGrassActor*> new_actors;

	// Prepare to spawn new actors and update old actors
	for (uint32_t i: grass_groups) {
		Closeness closeness;
		float dist;
		std::tie(closeness, dist) = GetCloseness(group_transforms[i], Planet, Players, false);

		float x = fmod(group_transforms[i].GetLocation().X, 2.0f);
		float y = fmod(group_transforms[i].GetLocation().Y, 2.0f);
		float z = fmod(group_transforms[i].GetLocation().Z, 2.0f);
		if (x < 0) x += 4;
		if (y < 0) y += 4;
		if (z < 0) z += 4;
		uint32_t sum = static_cast<uint32_t>(x) + static_cast<uint32_t>(y) + static_cast<uint32_t>(z);
		bool even = (sum % 2 == 0);

		if (closeness == Closeness::Close || (closeness == Closeness::Near && even)) {
			auto old_actor_it = this->GrassActors.find(i);
			if (old_actor_it != this->GrassActors.end()) {
				// Existing
				old_actor_it->second->SetFillStatus(PointFillStatus::Grass);
				if (footstep_groups.find(i) != footstep_groups.end()) {
					old_actor_it->second->Footstep();
				}

				new_actors[i] = old_actor_it->second;
			}
			else {
				// New
				actors_to_spawn.push_back(std::make_pair(i, dist));
			}
		}
	}

	// Sort actors to spawn
	std::sort(actors_to_spawn.begin(), actors_to_spawn.end(), [](const auto& a, const auto& b) { return a.second < b.second; });

	// Remove unused actors
	for (auto& actor : this->GrassActors) {
		if (new_actors.find(actor.first) == new_actors.end()) {
			actor.second->Destroy();
		}
	}

	// Spawn a limited number of new actors
	for (int i = 0; i < actors_to_spawn.size() && i < 20; i++) {
		auto group_transform = group_transforms[actors_to_spawn[i].first];
		auto group_scale = group_transform.GetScale3D();	// Set scale separately on components
		group_transform.SetScale3D(FVector::OneVector);

		AGrassActor* actor = world->SpawnActor<AGrassActor>(this->GrassActorClass, group_transform * Planet->ActorToWorld());
		for (auto c : actor->GetComponentsByClass(UStaticMeshComponent::StaticClass())) {
			auto component = dynamic_cast<UStaticMeshComponent*>(c);

			auto transform = component->GetComponentTransform();
			auto scale = transform.GetScale3D();
			scale.X *= group_scale.X;
			scale.Y *= group_scale.Y;
			scale.Z *= group_scale.Z;
			transform.SetScale3D(scale);

			component->SetWorldTransform(transform);
		}
		actor->SetFillStatus(PointFillStatus::Grass);
		if (footstep_groups.find(actors_to_spawn[i].first) != footstep_groups.end()) {
			actor->Footstep();
		}

		new_actors[actors_to_spawn[i].first] = actor;
	}
	this->GrassActors = std::move(new_actors);
}

void AGrassSpawnActor::ClearFootsteps(AMapReaderActor* MapReader)
{
	auto world = GetWorld();
	if (!world) return;

	auto& points = MapReader->GetMap();
	for (auto& point : points) {
		point.footstep = false;
	}
}

void AGrassSpawnActor::SetGrassActorClass(UClass *Class)
{
	this->GrassActorClass = Class;
}
