#pragma once

#include "MapReaderActor.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include <limits>

#include "GraphState.generated.h"


/**
 * 
 */
UCLASS()
class MYPROJECT4_API AGraphState : public AActor
{
public:
	// Sets default values for this actor's properties
	AGraphState();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	GENERATED_BODY()
	UFUNCTION(BlueprintCallable,
		meta = (DisplayName = "Step On",
			CompactNodeTitle = "stepOn",
			Keywords = "graph grass step"),
		Category = Game)
	void stepOn(AMapReaderActor *map_reader, FVector local_position, int32 max_fill);

private:
	uint32_t LastStep{ std::numeric_limits<uint32_t>::max() };

	void stepOnWithoutGrouping(AMapReaderActor* map_reader, FVector local_position, int32 max_fill);
	void adjustGroups(AMapReaderActor* map_reader);
};
