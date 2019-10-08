// Fill out your copyright notice in the Description page of Project Settings.


#include "generate_graph.h"
#include "Containers/array.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsPublic.h"
#include "PhysXIncludes.h"
#include "geometry/PxTriangleMesh.h"
#include "foundation/PxSimpleTypes.h"




// 1. Because we use graph to represent data, we have to ensure the player
//    never go from one point to a non-neighboring point. And grass can only be
//    filled by neighboring points.

template<typename index_type>
bool node_relation(index_type triangle_array,int triangle_A, int triangle_B,int required_shared_edges)
{
	auto VertA1 = ;
	auto VertA2 = triangle_array[triangle_A * 3 + 1];
	auto VertA3 = triangle_array[triangle_A * 3 + 2];

	int shared_edges = 0;

	for (int a = 0; a < 3; a++)
	{
		int(b = 0; b < 3; b++)
		{
			if (triangle_array[triangle_A * 3 + a]
			 == triangle_array[triangle_B * 3 + b])
			{
				shared_edges++;
			}
		}
	}

	return shared_edges >= required_shared_edges;
	
}


void fill_edges(TArray<Point> graph,physx::PxTriangleMesh *triangles)
{
	auto triangle_array = triangles->getTriangles();
	for (int tri = 0; tri < triangles->getNbTriangles(); tri++)
	{
		for (int vertex = 0; vertex < 3; vertex++)
		{
			for (int potential_neighbor = 0;
				potential_neighbor < triangles->getNbTriangles(); potential_neighbor++)
			{
				if (tri != potential_neighbor)
				{
					if (triangles->getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES)
					{
						if (node_relation<PxU16*>((PxU16*)triangle_array, tri, potential_neighbor,2))
						{
							graph[tri].next.AddUnique(potential_neighbor);
						}
					}
					else
					{
						if (node_relation((PxU32*)triangle_array, tri, potential_neighbor,2))
						{
							graph[tri].next.AddUnique(potential_neighbor);
						}
					}
				}
			}
		}
	}
}

TArray<Point,FDefaultAllocator> Ugenerate_graph::make_graph(UStaticMeshComponent mesh)
{
	physx::PxTriangleMesh *triangles = mesh.BodyInstance.BodySetup.Get()->TriMeshes[0];

	TArray<Point> graph = TArray<Point>();
	graph.Init(Point(),triangles->getNbTriangles());

	fill_edges(graph,triangles);

	return graph;
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

