#include "MapVersionSubsystem.h"
#include "Json.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

static FString GetPlatformFolder()
{
#if PLATFORM_ANDROID
	return TEXT("android");
#elif PLATFORM_IOS
	return TEXT("ios");
#else
	return TEXT("windows");
#endif
}

static FString GetPakSuffix()
{
#if PLATFORM_ANDROID
	return TEXT("Android_ASTC");
#elif PLATFORM_IOS
	return TEXT("iOS");
#else
	return TEXT("Windows");
#endif
}

void UMapVersionSubsystem::LoadMapData(const FString& BaseURL)
{
	TSharedRef<IHttpRequest> Req = FHttpModule::Get().CreateRequest();
	Req->SetURL(BaseURL + TEXT("maps.json"));
	Req->SetVerb(TEXT("GET"));
	Req->OnProcessRequestComplete().BindUObject(
		this, &UMapVersionSubsystem::OnServerResponse);
	Req->ProcessRequest();
}

void UMapVersionSubsystem::OnServerResponse(
	FHttpRequestPtr,
	FHttpResponsePtr Response,
	bool bSuccess)
{
	if (!bSuccess || !Response.IsValid()) return;

	TMap<FString, int32> LocalVersions = LoadLocalVersions();

	TSharedPtr<FJsonObject> Root;
	auto Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (!FJsonSerializer::Deserialize(Reader, Root)) return;

	const TArray<TSharedPtr<FJsonValue>>* MapsArray;
	if (!Root->TryGetArrayField("maps", MapsArray)) return;

	TArray<FMapInfo> Result;

	for (auto& Item : *MapsArray)
	{
		auto Obj = Item->AsObject();
		if (!Obj) continue;

		FMapInfo Info;
		Info.MapId = Obj->GetStringField("id");
		Info.DisplayName = FText::FromString(Obj->GetStringField("displayName"));
		Info.MapPath = Obj->GetStringField("mapPath");
		Info.LatestVersion = Obj->GetIntegerField("latestVersion");
		Info.ChunkId = Obj->GetIntegerField("chunkId");

		Info.InstalledVersion =
			LocalVersions.Contains(Info.MapId)
			? LocalVersions[Info.MapId]
			: 0;

		Info.bNeedsUpdate = Info.LatestVersion > Info.InstalledVersion;
		Result.Add(Info);
	}

	OnMapDataReady.Broadcast(Result);
}

FString UMapVersionSubsystem::BuildPakURL(
	const FString& BaseURL,
	int32 ChunkId,
	int32 Version) const
{
	return FString::Printf(
		TEXT("%s%s/v%d/pakchunk%d-%s.pak"),
		*BaseURL,
		*GetPlatformFolder(),
		Version,
		ChunkId,
		*GetPakSuffix()
	);
}

void UMapVersionSubsystem::MarkMapInstalled(
	const FString& MapId,
	int32 Version)
{
	TMap<FString, int32> Versions = LoadLocalVersions();
	Versions.Add(MapId, Version);
	SaveLocalVersions(Versions);
}

TMap<FString, int32> UMapVersionSubsystem::LoadLocalVersions() const
{
	TMap<FString, int32> Result;

	FString Path = GetLocalJsonPath();
	FString Text;
	if (!FFileHelper::LoadFileToString(Text, *Path)) return Result;

	TSharedPtr<FJsonObject> Root;
	auto Reader = TJsonReaderFactory<>::Create(Text);
	if (!FJsonSerializer::Deserialize(Reader, Root)) return Result;

	const TArray<TSharedPtr<FJsonValue>>* Maps;
	if (!Root->TryGetArrayField("maps", Maps)) return Result;

	for (auto& Item : *Maps)
	{
		auto Obj = Item->AsObject();
		if (!Obj) continue;

		Result.Add(
			Obj->GetStringField("id"),
			Obj->GetIntegerField("installedVersion")
		);
	}

	return Result;
}

void UMapVersionSubsystem::SaveLocalVersions(
	const TMap<FString, int32>& Versions)
{
	FString Dir = FPaths::ProjectSavedDir() / TEXT("DLC");
	IFileManager::Get().MakeDirectory(*Dir, true);

	TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();
	TArray<TSharedPtr<FJsonValue>> Array;

	for (auto& Pair : Versions)
	{
		auto Obj = MakeShared<FJsonObject>();
		Obj->SetStringField("id", Pair.Key);
		Obj->SetNumberField("installedVersion", Pair.Value);
		Array.Add(MakeShared<FJsonValueObject>(Obj));
	}

	Root->SetArrayField("maps", Array);

	FString Output;
	auto Writer = TJsonWriterFactory<>::Create(&Output);
	FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);

	FFileHelper::SaveStringToFile(Output, *GetLocalJsonPath());
}

FString UMapVersionSubsystem::GetLocalJsonPath() const
{
	return FPaths::ProjectSavedDir() / TEXT("DLC/local_maps.json");
}