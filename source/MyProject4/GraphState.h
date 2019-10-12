#pragma once

#include "generate_graph.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GraphState.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT4_API UGraphState : public UBlueprintFunctionLibrary
{
public:
	GENERATED_BODY()
	UFUNCTION(BlueprintCallable,
		meta = (DisplayName = "Step On",
			CompactNodeTitle = "stepOn",
			Keywords = "graph grass step"),
		Category = Game)
	static void StepOn(UPARAM(ref) TArray<FPoint>& Points, int32 Index, int32 MaxFill);
};
