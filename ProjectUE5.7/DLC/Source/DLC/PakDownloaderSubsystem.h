#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Http.h"
#include "PakDownloaderSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPakDownloaded);

UCLASS()
class DLC_API UPakDownloaderSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void DownloadPak(
		const FString& PakURL,
		const FString& MapId,
		int32 ChunkId,
		int32 Version
	);

	UPROPERTY(BlueprintAssignable)
	FOnPakDownloaded OnFinished;

private:
	FString SavedPakPath;
	FString PendingMapId;
	int32 PendingVersion = 0;

	void OnResponse(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess);
};