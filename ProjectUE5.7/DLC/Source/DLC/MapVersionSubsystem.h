#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Http.h"
#include "MapVersionSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FMapInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FString MapId;
	UPROPERTY(BlueprintReadOnly) FText DisplayName;
	UPROPERTY(BlueprintReadOnly) FString MapPath;
	UPROPERTY(BlueprintReadOnly) int32 LatestVersion = 0;
	UPROPERTY(BlueprintReadOnly) int32 InstalledVersion = 0;
	UPROPERTY(BlueprintReadOnly) int32 ChunkId = -1;
	UPROPERTY(BlueprintReadOnly) bool bNeedsUpdate = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnMapDataReady,
	const TArray<FMapInfo>&,
	Maps
);

UCLASS()
class DLC_API UMapVersionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void LoadMapData(const FString& BaseURL);

	UFUNCTION(BlueprintCallable)
	FString BuildPakURL(const FString& BaseURL, int32 ChunkId, int32 Version) const;

	UFUNCTION(BlueprintCallable)
	void MarkMapInstalled(const FString& MapId, int32 Version);

	UPROPERTY(BlueprintAssignable)
	FOnMapDataReady OnMapDataReady;

private:
	void OnServerResponse(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess);

	TMap<FString, int32> LoadLocalVersions() const;
	void SaveLocalVersions(const TMap<FString, int32>& Versions);
	FString GetLocalJsonPath() const;
};