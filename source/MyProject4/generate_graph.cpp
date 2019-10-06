// Fill out your copyright notice in the Description page of Project Settings.


#include "generate_graph.h"
#include "Containers/array.h"
#include "Components/StaticMeshComponent.h"


enum class PointFillStatus {
	Empty,
	Path,
	Grass
};

// 1. Because we use graph to represent data, we have to ensure the player
//    never go from one point to a non-neighboring point. And grass can only be
//    filled by neighboring points.
struct Point {
	PointFillStatus fill_status;
	bool has_cow;
	TArray<uint32_t> next;
	Point() : fill_status(PointFillStatus::Empty), has_cow() {}
};


TArray<Point> Ugenerate_graph::make_graph(UStaticMeshComponent mesh)
{
	return NULL;
}

// Sets default values for this component's properties
Ugenerate_graph::Ugenerate_graph()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void Ugenerate_graph::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}




// Called every frame
void Ugenerate_graph::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

