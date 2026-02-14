#include "PakDownloaderSubsystem.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "IPlatformFilePak.h"
#include "MapVersionSubsystem.h"

void UPakDownloaderSubsystem::DownloadPak(
	const FString& PakURL,
	const FString& MapId,
	int32 ChunkId,
	int32 Version)
{
	PendingMapId = MapId;
	PendingVersion = Version;

	FString Dir = FPaths::ProjectPersistentDownloadDir();
	IFileManager::Get().MakeDirectory(*Dir, true);

	SavedPakPath = Dir / FString::Printf(TEXT("pakchunk%d.pak"), ChunkId);

	TSharedRef<IHttpRequest> Req = FHttpModule::Get().CreateRequest();
	Req->SetURL(PakURL);
	Req->SetVerb(TEXT("GET"));
	Req->OnProcessRequestComplete().BindUObject(
		this, &UPakDownloaderSubsystem::OnResponse);
	Req->ProcessRequest();
}

void UPakDownloaderSubsystem::OnResponse(
	FHttpRequestPtr,
	FHttpResponsePtr Response,
	bool bSuccess)
{
	if (!bSuccess || !Response.IsValid()) return;

	FFileHelper::SaveArrayToFile(
		Response->GetContent(), *SavedPakPath);

	IPlatformFile& Base = FPlatformFileManager::Get().GetPlatformFile();
	auto PakPlatform = new FPakPlatformFile();
	PakPlatform->Initialize(&Base, TEXT(""));
	FPlatformFileManager::Get().SetPlatformFile(*PakPlatform);

	PakPlatform->Mount(*SavedPakPath, 0, *FPaths::ProjectContentDir());

	if (auto* MapSys = GetGameInstance()->GetSubsystem<UMapVersionSubsystem>())
	{
		MapSys->MarkMapInstalled(PendingMapId, PendingVersion);
	}

	OnFinished.Broadcast();
}