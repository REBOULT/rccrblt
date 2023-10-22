// жирная мать какраза не учила его писать обход на байты
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <sstream>


std::vector<byte> HexStringToBytes(const std::string& hexString) {
    std::vector<byte> bytes;
    std::istringstream hexStream(hexString);

    unsigned int byteValue;
    while (hexStream >> std::hex >> byteValue) {
        bytes.push_back(static_cast<byte>(byteValue));
    }

    return bytes;
}

bool FindBytesInMemory(HANDLE processHandle, const std::vector<byte>& targetBytes) {
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    MEMORY_BASIC_INFORMATION memoryInfo;

    byte* buffer = nullptr;
    size_t bytesRead = 0;

    for (byte* address = (byte)systemInfo.lpMinimumApplicationAddress;
        address < (byte)systemInfo.lpMaximumApplicationAddress;
        address = (byte)memoryInfo.BaseAddress + memoryInfo.RegionSize) {
        if (VirtualQueryEx(processHandle, address, &memoryInfo, sizeof(memoryInfo)) == sizeof(memoryInfo)) {
            if (memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS) {
                buffer = new byte[memoryInfo.RegionSize];
                ReadProcessMemory(processHandle, memoryInfo.BaseAddress, buffer, memoryInfo.RegionSize, &bytesRead);

                if (bytesRead > 0) {
                    for (size_t i = 0; i <= bytesRead - targetBytes.size(); i++) {
                        bool found = true;
                        for (size_t j = 0; j < targetBytes.size(); j++) {
                            if (buffer[i + j] != targetBytes[j]) {
                                found = false;
                                break;
                            }
                        }

                        if (found) {
                            delete[] buffer;
                            return true;
                        }
                    }
                }

                delete[] buffer;
                buffer = nullptr;
            }
        }
    }

    return false;
}
// ищем ебанный инвиз с супериорити
int main() {
    const std::string ebanniecifri = "73 75 70 65 72 69 6f 72 69 74 79";
    const std::vector<byte> targetBytes = HexStringToBytes(ebanniecifri);

    const wchar_t processName = L"RustClient.exe";

    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W processEntry;
        processEntry.dwSize = sizeof(processEntry);

        if (Process32FirstW(snapshot, &processEntry)) {
            do {
                if (wcscmp(processEntry.szExeFile, processName) == 0) {
                    processId = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &processEntry));
        }

        CloseHandle(snapshot);
    }

    if (processId != 0) {
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);

        if (processHandle != NULL) {
            if (FindBytesInMemory(processHandle, targetBytes)) {
                std::wcout << L"Всё хуёво братан" << std::endl;
            }
            else {
                std::wcout << L"Всё еще хуже, блять" << std::endl;
            }

            CloseHandle(processHandle);
        }
        else {
            std::cerr << "Да снова хуйня, сука, ебанный рот. Ошибка: " << GetLastError() << std::endl;
        }
    }
    else {
        std::wcerr << L"Запусти игру, долбоеб" << std::endl;
    }

 //xd
    Sleep(10000);

    return 0;
}