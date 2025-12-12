#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <conio.h>
#include "steam/steam_api.h"

void SetSteamAppId(const std::string& appId) {
    SetEnvironmentVariableA("SteamAppId", appId.c_str());
}

std::vector<int> ReadGameIds(const std::string& filename) {
    std::vector<int> ids;
    std::ifstream file(filename);
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            if (!line.empty()) {
                try {
                    int id = std::stoi(line);
                    if (id > 0) ids.push_back(id);
                } catch (...) {}
            }
        }
        file.close();
    }
    return ids;
}

int RunChild(int appId) {
    SetSteamAppId(std::to_string(appId));

    if (!SteamAPI_Init()) {
        std::cerr << "[Child " << appId << "] Failed to initialize SteamAPI. Is Steam running?" << std::endl;
        return 1;
    }

    std::cout << "[Child " << appId << "] Idling game..." << std::endl;

    while (true) {
        SteamAPI_RunCallbacks();
        Sleep(1000);
    }

    SteamAPI_Shutdown();
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 3 && std::string(argv[1]) == "--child") {
        int appId = std::stoi(argv[2]);
        return RunChild(appId);
    }

    std::cout << "Steam Idler Manager" << std::endl;
    std::cout << "===================" << std::endl;

    // 1. Initialize SteamAPI with a generic ID (Spacewar - 480) to check ownership
    // We use 480 because it's available to everyone and allows us to access ISteamApps
    SetSteamAppId("480");
    
    if (!SteamAPI_Init()) {
        std::cerr << "Fatal Error: Failed to initialize SteamAPI. Please ensure Steam is running and you are logged in." << std::endl;
        std::cerr << "Also ensure steam_api64.dll is in the same folder as this executable." << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    std::vector<int> gameIds = ReadGameIds("games.cfg");
    if (gameIds.empty()) {
        std::cout << "No game IDs found in games.cfg." << std::endl;
        SteamAPI_Shutdown();
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 0;
    }

    // ownership check
    char selfPath[MAX_PATH];
    GetModuleFileNameA(NULL, selfPath, MAX_PATH);

    ISteamApps* pApps = SteamApps();
    if (!pApps) {
        std::cerr << "Error: Could not get ISteamApps interface." << std::endl;
        return 1;
    }

    while (true) {
        std::vector<PROCESS_INFORMATION> children;
        int count = 0;
        for (int appId : gameIds) {
            if (pApps->BIsSubscribedApp(appId)) {
                std::cout << "Verified ownership of AppID " << appId << ". Launching idler..." << std::endl;

                std::string cmdLine = "\"" + std::string(selfPath) + "\" --child " + std::to_string(appId);
                
                STARTUPINFOA si;
                PROCESS_INFORMATION pi;
                ZeroMemory(&si, sizeof(si));
                si.cb = sizeof(si);
                ZeroMemory(&pi, sizeof(pi));

                if (CreateProcessA(NULL, const_cast<char*>(cmdLine.c_str()), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
                    children.push_back(pi);
                    count++;
                } else {
                    std::cerr << "Failed to launch child process for " << appId << std::endl;
                }
            } else {
                std::cout << "Skipping AppID " << appId << " (Not owned)" << std::endl;
            }
        }

        if (count == 0) {
            std::cout << "No valid games to idle." << std::endl;
            break;
        } else {
            std::cout << "\nIdling " << count << " games." << std::endl;
            std::cout << "Restarting in 3 hours. Press any key to stop all idlers and exit." << std::endl;
            
            bool stop = false;
            // 3 hours = 10800 seconds
            for (int i = 0; i < 10800; ++i) {
                if (_kbhit()) {
                    stop = true;
                    _getch(); // consume the key
                    break;
                }
                Sleep(1000);
            }

            std::cout << (stop ? "Stopping idlers..." : "Restarting idlers...") << std::endl;
            for (const auto& pi : children) {
                TerminateProcess(pi.hProcess, 0);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }

            if (stop) break;
        }
    }

    SteamAPI_Shutdown();
    return 0;
}
