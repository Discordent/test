#pragma once

#include "Types.generated.h"

USTRUCT(BlueprintType)
struct FTokenState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Access_Token;

	UPROPERTY(BlueprintReadWrite)
	FString Refresh_Token;

	UPROPERTY(BlueprintReadWrite)
	FDateTime Expired_At;
};
