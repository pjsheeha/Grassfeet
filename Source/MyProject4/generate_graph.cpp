#include "generate_graph.h"
#include "Containers/array.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsPublic.h"
#include "PhysXIncludes.h"
#include "geometry/PxTriangleMesh.h"
#include "foundation/PxSimpleTypes.h"
#include "Engine/StaticMesh.h"
#include "Kismet/KismetMathLibrary.h"
#include "StaticMeshResources.h"


// 1. Because we use graph to represent data, we have to ensure the player
//    never go from one point to a non-neighboring point. And grass can only be
//    filled by neighboring points.

bool node_relation(TArray<FVector> vertices,TArray<int> triangles ,int triangle_A, int triangle_B,int required_shared_edges)
{
	int32 shared_edges = 0;

	for (int a = 0; a < 3; a++)
	{
		for(int b = 0; b < 3; b++)
		{
			if ( (vertices[triangles[triangle_A * 3 + a]]
			 - vertices[triangles[triangle_B * 3 + b]]).Size() < .01f)
			{
				shared_edges++;
			}
		}
	}

	return shared_edges >= required_shared_edges;
}

void fill_edges(TArray<FPoint> &graph,physx::PxTriangleMesh *triangles)
{
	/*auto triangle_array = triangles->getTriangles();
	for (int32 tri = 0; tri < (int32)triangles->getNbTriangles(); tri++)
	{
		for (int32 potential_neighbor = 0;
			potential_neighbor < (int32)triangles->getNbTriangles(); potential_neighbor++)
		{
			if (tri != potential_neighbor)
			{
				if (triangles->getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES)
				{
					if (node_relation((PxU16*)triangle_array, tri, potential_neighbor,2))
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
	}*/
}

PointFillStatus Ugenerate_graph::IsGrass(TArray<FPoint> graph, int index)
{
	return graph[index].fill_status;
}

void Ugenerate_graph::AddAllAdjacency(UPARAM(ref) TArray<FPoint>& graph, TArray<FVector> vertices, TArray<FVector> normals, TArray<int> triangles)
{
	for (int32 tri = 0; tri < triangles.Num() / 3; tri++)
	{
		FTransform transform;
		transform.SetLocation((vertices[triangles[tri * 3 + 0]] + vertices[triangles[tri * 3 + 1]] + vertices[triangles[tri * 3 + 2]]) / 3);
		transform.SetRotation(FQuat(UKismetMathLibrary::MakeRotFromZ((normals[triangles[tri * 3 + 0]] + vertices[triangles[tri * 3 + 1]] + vertices[triangles[tri * 3 + 2]]) / 3)));
		graph[tri].transform = transform;

		for (int32 potential_neighbor = 0;
			potential_neighbor < triangles.Num() / 3; potential_neighbor++)
		{
			if (tri != potential_neighbor)
			{
				if (node_relation(vertices, triangles, tri, potential_neighbor, 2))
				{
					graph[tri].next.AddUnique(potential_neighbor);
				}
				
			}
		}
	}
}
/*
void Ugenerate_graph::AddAdjacency(UPARAM(ref) TArray<FPoint>& graph, int a, int b)
{
	graph[a].next.AddUnique(b);
	graph[b].next.AddUnique(a);
}*/

TArray<FPoint,FDefaultAllocator> Ugenerate_graph::make_graph(UStaticMeshComponent *mesh)
{
	auto body = mesh->BodyInstance;
	auto setup = body.BodySetup.Get();
	physx::PxTriangleMesh *triangles = setup->TriMeshes[0];


	TArray<FPoint> graph = TArray<FPoint>();
	graph.Init(FPoint(),triangles->getNbTriangles());
	/*
	auto staticmesh = mesh->GetStaticMesh();
	auto renderData = staticmesh->RenderData->LODResources;
	auto section = renderData[0].Sections[0];*/


	fill_edges(graph, triangles);

	/*UE_LOG(LogTemp, Warning, TEXT("GraphSize: %d"), graph.Num());
	for (auto& x : graph) {
		UE_LOG(LogTemp, Warning, TEXT("NextSize: %d"), x.next.Num());
		for (auto& y : x.next) {
			UE_LOG(LogTemp, Warning, TEXT("Next: %d"), y);
		}
		UE_LOG(LogTemp, Warning, TEXT("Next: ==================="));
	}*/

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

