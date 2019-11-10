 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MapReaderActor.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CowComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT4_API UCowComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCowComponent();
	UFUNCTION(BlueprintCallable,
		meta = (DisplayName = "Eat Grass",
			CompactNodeTitle = "Eat Grass",
			Keywords = "cow eat grass"),
		Category = Game)
		void EatGrass( AMapReaderActor* map_reader, FVector local_position, float eat_range);

	UFUNCTION(BlueprintCallable,
		meta = (DisplayName = "Cow Step",
			CompactNodeTitle = "Cow Step",
			Keywords = "cow step move"),
		Category = Game)
		void CowStep(AMapReaderActor* map_reader, FVector local_position, int cow_id);

	UFUNCTION(BlueprintCallable,
		meta = (DisplayName = "On Grass",
			CompactNodeTitle = "On Grass",
			Keywords = "cow on grass"),
		Category = Game)
		bool OnGrass(AMapReaderActor* map_reader, FVector local_position);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	uint32 prev_step{ std::numeric_limits<uint32_t>::max() };


		
};
