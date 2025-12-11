# steam-idler-cpp

A lightweight C++ application that mimics Steam games. It connects to your running Steam client to idle games without requiring your password.

## Prerequisites

1.  **Visual Studio**: You need Visual Studio with C++ workload installed to build the project.
2.  **Steamworks SDK**: You must download the Steamworks SDK from Valve.

## Setup Instructions

1.  **Download Steamworks SDK**:
    *   Go to [https://partner.steamgames.com/doc/sdk](https://partner.steamgames.com/doc/sdk) and download the latest SDK zip file.
2.  **Extract SDK**:
    *   Extract the contents of the zip file.
    *   Copy the `sdk` folder into `src/external/` inside the project.
    *   Your folder structure should look like this:
        ```
        steam-idler/
        ├── games.cfg
        ├── SteamIdler.sln
        ├── SteamIdler.vcxproj
        ├── src/
        │   ├── main.cpp
        │   └── external/
        │       └── sdk/
        │           ├── public/
        │           ├── redistributable_bin/
        │           └── ...
        ```
3.  **Build**:
    *   Open `SteamIdler.sln` in Visual Studio.
    *   Select **Release** and **x64** configuration.
    *   Build the solution (Ctrl+Shift+B).
    *   The executable will be created in `bin/Release/`.

## How do i use it?

1.  **Configure Games**:
    *   Edit `games.cfg` in the project root (or copy it to the same folder as the `.exe` if you move it).
    *   Add the AppIDs of the games you want to idle.
2.  **Run**:
    *   Make sure Steam is running and you are logged in.
    *   Run `SteamIdler.exe` (from `bin/Release/`).
    *   The program will check which games you own and spawn a separate window for each game to idle them.
    *   Keep the main window open. Press **Enter** in the main window to close all idlers.

## FAQ

*   **Missing steam_api64.dll**: The build process tries to copy this automatically. If it's missing, copy it manually from `sdk/redistributable_bin/win64/` to the folder where `SteamIdler.exe` is.
*   **Steam Init Failed**: Ensure Steam is running.
