// Fill out your copyright notice in the Description page of Project Settings.


#include "generate_graph.h"
#include "Containers/array.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"




// 1. Because we use graph to represent data, we have to ensure the player
//    never go from one point to a non-neighboring point. And grass can only be
//    filled by neighboring points.


TArray<Ugenerate_graph::Point,FDefaultAllocator> Ugenerate_graph::make_graph(UStaticMeshComponent mesh)
{
	auto body_setup = mesh.BodyInstance.BodySetup.Get();
	auto tris = body_setup->TriMeshes[0];
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

