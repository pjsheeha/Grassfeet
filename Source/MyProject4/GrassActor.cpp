#include "GrassActor.h"

#include "generate_graph.h"
#include "Util.h"

#include "EngineUtils.h"
#include "GameFramework/Controller.h"

#include <memory>

// Sets default values
AGrassActor::AGrassActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGrassActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGrassActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGrassActor::UpdateGrass(UPARAM(ref) TArray<FPoint>& points)
{
	auto world = GetWorld();
	if (!world) return;

	auto controller = world->GetFirstPlayerController<AController>();
	if (!controller) return;
	auto pawn = controller->GetPawn();
	if (!pawn) return;

	UClass *grass_class = FindObject<UClass>(ANY_PACKAGE, L"/Game/TwinStickBP/Blueprints/Grass.Grass_C");
	//AActor *grass = world->SpawnActor<AActor>(grass_class, pawn->GetTransform());
	//grass->Destroy();
}
