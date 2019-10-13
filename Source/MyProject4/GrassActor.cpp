#include "generate_graph.h"
#include "GrassActor.h"
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
	
	auto world = GetWorld();
	if (!world) return;
	auto controller = world->GetFirstPlayerController<AController>();
	if (!controller) return;
	auto pawn = controller->GetPawn();
	if (!pawn) return;

	UArrayProperty* prop = FindField<UArrayProperty>(pawn->GetClass(), L"graph");
	if (!prop) return;
	FScriptArray *script_array = prop->GetPropertyValuePtr_InContainer(pawn);
	if (!script_array) return;

	// This is ugly, but by design this is how you do it in Unreal.
	auto &array = *reinterpret_cast<TArray<FPoint>*>(script_array);

	UClass* grass = FindObject<UClass>(ANY_PACKAGE, L"/Game/TwinStickBP/Blueprints/Grass.Grass_C");
	GetWorld()->SpawnActor<UObject>(grass, pawn->GetTransform());
}
