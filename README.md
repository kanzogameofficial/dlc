
Video: https://www.youtube.com/watch?v=MMg0mkJ5v-M

🚀 Unreal Engine On-Demand Map Download System (DLC / PAK Streaming)

This repository contains a fully working on-demand map download system for Unreal Engine 5.x using:

HTTP hosted .pak files

JSON-based version control

Runtime PAK mounting

Persistent install state (no re-download after restart)

UI-driven download / play flow

Designed for Windows & Android (ASTC) and easily extendable to iOS.

📐 SYSTEM ARCHITECTURE
Server (HTTP)
 ├── maps.json
 ├── windows/
 │    └── v1/
 │         └── pakchunk1-Windows.pak
 ├── android/
 │    └── v1/
 │         └── pakchunk1-Android_ASTC.pak
 └── ios/
      └── v1/
           └── pakchunk1-iOS.pak
Unreal Game
 ├── MapVersionSubsystem (JSON + versioning)
 ├── PakDownloaderSubsystem (Download + Mount)
 ├── UI
 │    ├── WBP_MapList
 │    └── WBP_MapRow
 └── Saved/DLC/local_maps.json (installed state)
🌐 BASE URL (SERVER ROOT)

Use ONLY root URL, never full file paths.

Example (Local XAMPP)
http://127.0.0.1/dlc
Example (GitHub CDN via jsDelivr)
https://cdn.jsdelivr.net/gh/kanzogameofficial/dlc@main

⚠️ Note: jsDelivr has caching. For instant updates use:

XAMPP

Cloudflare Pages

Netlify

Firebase Hosting

📄 SERVER JSON FORMAT (maps.json)
{
  "maps": [
    {
      "id": "City",
      "displayName": "City Map",
      "mapPath": "/Game/Maps/CityMap",
      "latestVersion": 1,
      "chunkId": 1
    },
    {
      "id": "Desert",
      "displayName": "Desert Map",
      "mapPath": "/Game/Maps/DesertMap",
      "latestVersion": 1,
      "chunkId": 2
    }
  ]
}
Field Explanation
Field	Description
id	Unique map identifier
displayName	UI name
mapPath	Unreal travel path
latestVersion	Version on server
chunkId	Chunk / PAK ID
🧠 CORE SUBSYSTEMS
1️⃣ MapVersionSubsystem

Responsibilities:

Download maps.json

Compare server vs local versions

Decide Install / Update / Play

Save installed versions to disk

Local file:

Saved/DLC/local_maps.json

Example:

{
  "maps": [
    { "id": "City", "installedVersion": 1 }
  ]
}
2️⃣ PakDownloaderSubsystem

Responsibilities:

Download .pak via HTTP

Save to persistent storage

Mount PAK at runtime

Fire OnFinished event

Mount location:

ProjectPersistentDownloadDir()
🎮 UI ARCHITECTURE
WBP_MapList

Calls LoadMapData(BaseURL)

Receives OnMapDataReady(Maps)

Creates WBP_MapRow per map

WBP_MapRow (Single Card)

Variables:

Name	Type	Expose on Spawn
MapInfo	FMapInfo	✅
BaseURL	String	✅
Button Logic

Download

Show "Downloading..."

Disable button

Call DownloadPak()

On Download Finished

MarkMapInstalled()

InitRow() (refresh UI)

Play

OpenLevel(MapInfo.MapPath)

🔁 UI FLOW (IMPORTANT)
Game Start
 └── LoadMapData
      └── JSON parsed
           └── UI Cards created
                ├── Download
                ├── Downloading...
                └── Play
Download Click
 ├── UI shows "Downloading..."
 ├── Pak download starts
 └── OnFinished
      ├── Save installed version
      └── Refresh UI → "Play"
📦 HOW TO CREATE PAK FILES (STEP BY STEP)
1️⃣ Assign Maps to Chunks

CityMap → Chunk ID 1

DesertMap → Chunk ID 2

Use:

Primary Asset Labels
or

Packaging settings → Map list per chunk

2️⃣ Packaging Settings
Project Settings → Packaging

Enable:

✅ Use Pak File

✅ Generate Chunks

✅ Build HTTP Chunk Install Data

Set:

Http Chunk Install Data Directory = ChunkOutput
Http Chunk Install Data Version   = v1
3️⃣ Build Platform

Windows → Shipping

Android → ASTC

4️⃣ Find Output
<ProjectRoot>/ChunkOutput/
 ├── Windows/v1/pakchunk1-Windows.pak
 ├── Android_ASTC/v1/pakchunk1-Android_ASTC.pak
5️⃣ Upload to Server

Upload only platform folder, NOT ChunkOutput root.

/dlc/
 ├── maps.json
 ├── windows/v1/*.pak
 ├── android/v1/*.pak
 └── ios/v1/*.pak
🧪 TESTING CHECKLIST

 Download shows "Downloading..."

 Download completes → Play appears

 App restart → still Play

 No duplicate downloads

 Correct map opens

 Platform-specific pak auto selected

🚫 COMMON MISTAKES

❌ Calling InitRow() only once
❌ Using full pak URL instead of BaseURL
❌ Hardcoding platform in Blueprint
❌ Mounting to wrong directory
❌ Not saving local install state

🏁 FINAL RESULT

✔ Fully dynamic
✔ Version controlled
✔ Platform aware
✔ Persistent
✔ Production-ready DLC system

📌 FUTURE IMPROVEMENTS

Download progress bar

Size estimation

Resume / cancel download

Delta patching

Encrypted PAKs
