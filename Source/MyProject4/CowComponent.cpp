// Fill out your copyright notice in the Description page of Project Settings.


#include "CowComponent.h"


#include <queue>

// Sets default values for this component's properties
UCowComponent::UCowComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCowComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCowComponent::EatGrass(AMapReaderActor* map_reader, FVector local_position, float eat_range) {
	auto& points = map_reader->GetMap();

	if (points.size() <= 0) {
		// Not initialized.
		return;
	}

	float min_dist = std::numeric_limits<float>::max();
	uint32_t min_i = 0;
	for (uint32_t i = 0; i < points.size(); i++) {
		FVector delta = points[i].transform.GetLocation() - local_position;
		float dist = delta.X * delta.X + delta.Y * delta.Y + delta.Z * delta.Z;
		if (dist < min_dist) {
			min_dist = dist;
			min_i = i;
		}
	}
	

	std::queue<uint32_t> eat_queue;
	std::vector<bool> visited(points.size());
	eat_queue.push(min_i);
	
	while (!eat_queue.empty()) {
		uint32_t current = eat_queue.front();
		eat_queue.pop();

		for (auto& p : points[current].next) {
			float dist = FVector::Dist(points[p].transform.GetLocation(),local_position);
			
			if ((dist <= eat_range)&&(!visited[p])) {

				eat_queue.push(p);
				visited[p] = true;
				points[p].fill_status = PointFillStatus::Empty;
			}

		}
	}
}

void UCowComponent::CowStep(AMapReaderActor* map_reader, FVector local_position,int cow_id) {
	auto& points = map_reader->GetMap();

	if (points.size() <= 0) {
		// Not initialized.
		return;
	}

	float min_dist = std::numeric_limits<float>::max();
	uint32_t min_i = 0;
	for (uint32_t i = 0; i < points.size(); i++) {
		FVector delta = points[i].transform.GetLocation() - local_position;
		float dist = delta.X * delta.X + delta.Y * delta.Y + delta.Z * delta.Z;
		if (dist < min_dist) {
			min_dist = dist;
			min_i = i;
		}
	}


	if (min_i == prev_step) {
		return;
	}
	else {
		points[min_i].cows.set(cow_id, true);

		if (prev_step <= points.size()) {
			points[prev_step].cows.set(cow_id, false);
		}
		prev_step = min_i;
	}
}


bool UCowComponent::OnGrass(AMapReaderActor* map_reader, FVector local_position) {
	auto& points = map_reader->GetMap();

	if (points.size() <= 0) {
		// Not initialized.
		return false;
	}

	float min_dist = std::numeric_limits<float>::max();
	uint32_t min_i = 0;
	for (uint32_t i = 0; i < points.size(); i++) {
		FVector delta = points[i].transform.GetLocation() - local_position;
		float dist = delta.X * delta.X + delta.Y * delta.Y + delta.Z * delta.Z;
		if (dist < min_dist) {
			min_dist = dist;
			min_i = i;
		}
	}

	return (points[min_i].fill_status != PointFillStatus::Empty);
}