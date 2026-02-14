🚀 Unreal Engine 5 – On-Demand Map Download System (DLC / PAK Streaming)

📺 Video Tutorial
👉 https://www.youtube.com/watch?v=MMg0mkJ5v-M

This repository contains a fully working on-demand map download (DLC) system for Unreal Engine 5.x, built using:

🌐 HTTP-hosted .pak files

📄 JSON-based version control

📦 Runtime PAK downloading & mounting

💾 Persistent install state (no re-download after restart)

🎮 UI-driven Download / Play flow

✅ Works on Windows & Android (ASTC)
🧩 Easily extendable to iOS

📐 System Architecture
🌍 Server (HTTP)
/dlc
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
🎮 Unreal Engine Project
/Game
 ├── Subsystems
 │    ├── MapVersionSubsystem
 │    └── PakDownloaderSubsystem
 ├── UI
 │    ├── WBP_MapList
 │    └── WBP_MapRow
 └── Saved/
      └── DLC/
           └── local_maps.json
🌐 Base URL (Server Root)

⚠️ Always pass ONLY the root folder URL
❌ Never use full file paths

✅ Local (XAMPP)
http://127.0.0.1/dlc
✅ GitHub CDN (jsDelivr)
https://cdn.jsdelivr.net/gh/kanzogameofficial/dlc@main

⚠️ jsDelivr caches aggressively

For instant updates, prefer:

XAMPP (local testing)

Cloudflare Pages

Netlify

Firebase Hosting

📄 Server JSON Format (maps.json)
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
displayName	Display name in UI
mapPath	Unreal level travel path
latestVersion	Server version
chunkId	Chunk / PAK ID
🧠 Core Subsystems
1️⃣ MapVersionSubsystem

Responsibilities

Download maps.json

Parse map metadata

Compare server vs local versions

Decide Download / Update / Play

Persist install state

📁 Local file:

Saved/DLC/local_maps.json

Example

{
  "maps": [
    { "id": "City", "installedVersion": 1 }
  ]
}
2️⃣ PakDownloaderSubsystem

Responsibilities

Download .pak files via HTTP

Save to persistent storage

Mount PAK at runtime

Broadcast OnFinished

📂 Mount location:

FPaths::ProjectPersistentDownloadDir()
🎮 UI Architecture
WBP_MapList

Calls LoadMapData(BaseURL)

Receives OnMapDataReady(Maps)

Creates WBP_MapRow per map

WBP_MapRow (Single Card)

Variables

Name	Type	Expose on Spawn
MapInfo	FMapInfo	✅
BaseURL	String	✅
Button Logic
📥 Download

Show "Downloading..."

Disable Download button

Call DownloadPak()

✅ On Download Finished

Call MarkMapInstalled(MapId, Version)

Call InitRow() to refresh UI

▶️ Play

OpenLevel(MapInfo.MapPath)

🔁 UI Flow (Very Important)
Game Start
 └── LoadMapData
      └── JSON parsed
           └── UI cards created
                ├── Download
                ├── Downloading...
                └── Play
Download Click
 ├── UI shows "Downloading..."
 ├── Pak download starts
 └── OnFinished
      ├── Save installed version
      └── Refresh UI → "Play"
📦 Creating PAK Files (Step-by-Step)
1️⃣ Assign Maps to Chunks

CityMap → Chunk ID 1

DesertMap → Chunk ID 2

Use either:

Primary Asset Labels

Packaging → Map list per chunk

2️⃣ Packaging Settings

Project Settings → Packaging

Enable:

✅ Use Pak File

✅ Generate Chunks

✅ Build HTTP Chunk Install Data

Set:

Http Chunk Install Data Directory = ChunkOutput
Http Chunk Install Data Version   = v1
3️⃣ Build Platforms

Windows → Shipping

Android → ASTC

4️⃣ Output Location
<ProjectRoot>/ChunkOutput/
 ├── Windows/v1/pakchunk1-Windows.pak
 ├── Android_ASTC/v1/pakchunk1-Android_ASTC.pak
5️⃣ Upload to Server

⚠️ Upload ONLY platform folders

/dlc
 ├── maps.json
 ├── windows/v1/*.pak
 ├── android/v1/*.pak
 └── ios/v1/*.pak

❌ Do NOT upload ChunkOutput root

🧪 Testing Checklist

✅ Download shows "Downloading..."
✅ Download finishes → Play appears
✅ App restart → Play still available
✅ No duplicate downloads
✅ Correct map loads
✅ Platform-specific pak auto selected

🚫 Common Mistakes

❌ Calling InitRow() only once
❌ Passing full pak URL instead of BaseURL
❌ Hardcoding platform in Blueprint
❌ Mounting to wrong directory
❌ Not saving install state

🏁 Final Result

✔ Fully dynamic
✔ Version controlled
✔ Platform aware
✔ Persistent across restarts
✔ Production-ready DLC system

📌 Future Improvements

Download progress bar

File size estimation

Resume / cancel downloads

Delta patching

Encrypted PAKs
