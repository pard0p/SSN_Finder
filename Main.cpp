#include "PEB.h"
#include <Windows.h>
#include <cstdio>

extern "C" DWORD GetSSNByFuncAddress(HANDLE functionAddress);

HMODULE GetNtdllHandle() {
    #if defined(_WIN64)
        PPEB Peb = (PPEB)__readgsqword(0x60);
    #else
        PPEB Peb = (PPEB)__readfsdword(0x30);
    #endif

    PLDR_MODULE pLoadModule;
    pLoadModule = (PLDR_MODULE)((PBYTE)Peb->LoaderData->InMemoryOrderModuleList.Flink->Flink - 16);

    return (HMODULE)pLoadModule->BaseAddress;
}

PIMAGE_EXPORT_DIRECTORY GetExportTableAddress(HMODULE ImageBase) {
    uintptr_t baseAddress = reinterpret_cast<uintptr_t>(ImageBase);

    uintptr_t dosHeaderAddr = baseAddress;
    IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(dosHeaderAddr);

    uintptr_t peHeaderAddr = baseAddress + dosHeader->e_lfanew;
    IMAGE_NT_HEADERS* ntHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(peHeaderAddr);

    IMAGE_EXPORT_DIRECTORY* exportDir = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(
        baseAddress + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    return (PIMAGE_EXPORT_DIRECTORY)exportDir;
}

HANDLE GetExportFunctionAddress(HMODULE moduleHandle, PIMAGE_EXPORT_DIRECTORY exportDir, const char* functionName) {
    uintptr_t baseAddress = reinterpret_cast<uintptr_t>(moduleHandle);
    DWORD* addressOfFunctions = reinterpret_cast<DWORD*>(baseAddress + exportDir->AddressOfFunctions);
    DWORD numberOfFunctions = exportDir->NumberOfFunctions;

    DWORD* addressOfNameOrdinals = reinterpret_cast<DWORD*>(baseAddress + exportDir->AddressOfNameOrdinals);
    DWORD* addressOfNames = reinterpret_cast<DWORD*>(baseAddress + exportDir->AddressOfNames);

    uintptr_t functionAddress = 0;

    for (DWORD i = 0; i < numberOfFunctions; ++i) {
        const char* currentFunctionName = nullptr;

        if (i < exportDir->NumberOfNames) {
            currentFunctionName = reinterpret_cast<const char*>(baseAddress + addressOfNames[i]);
        }

        functionAddress = baseAddress + addressOfFunctions[i+1];

        if (currentFunctionName && strcmp(currentFunctionName, functionName) == 0) {
            functionAddress = functionAddress;
            return (HANDLE)functionAddress;
        }
    }

    return (HANDLE)-1;
}

DWORD GetSSN(const char* functionName) {
    //1. Get a HANDLE to the NTDLL.
    HMODULE hNtdll = GetNtdllHandle();
    printf("NTDLL at: 0x%p\n", hNtdll);

    //2. Get NTDLL's export table.
    PIMAGE_EXPORT_DIRECTORY exportTable = GetExportTableAddress(hNtdll);
    printf("NTDLL export table at: 0x%p\n", exportTable);

    //3. Get the NTDLL's function address by its name.
    HANDLE functionAddress = GetExportFunctionAddress(hNtdll, exportTable, functionName);
    printf("Function at: 0x%p\n", functionAddress);

    //4. Get the Syscall number.
    DWORD SSN = GetSSNByFuncAddress(functionAddress);
    printf("SSN: 0x%x\n", SSN);

    return SSN;
}

int main() {
    GetSSN("NtWriteVirtualMemory");
    return 0;
}