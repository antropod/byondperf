#include <windows.h>
#include <byondapi.h>

struct DungServer;
typedef char* DMString; // it is not real char*, there is some meta data (ptr-4 is refcnt, ptr-8 is ???) so don't free() it.

#define LOAD_FUNCTION(dll, name) (name = (fp_##name)(GetProcAddress(dll, nm_##name)))

typedef DMString* (__thiscall* fp_DungServer_GetServerMemUsage)(DungServer* this_, DMString* outvar);
fp_DungServer_GetServerMemUsage DungServer_GetServerMemUsage;
constexpr const char* nm_DungServer_GetServerMemUsage = "?GetServerMemUsage@DungServer@@QAE?AUDMString@@XZ";

typedef void(__thiscall* fp_DMString_Destructor)(DMString* this_);
fp_DMString_Destructor DMString_Destructor;
constexpr const char* nm_DMString_Destructor = "??1DMString@@QAE@XZ";

bool LoadFunctions()
{
    HMODULE byondcore = GetModuleHandleA("byondcore.dll");
    if (!byondcore) {
        return false;
    }

    if (!LOAD_FUNCTION(byondcore, DungServer_GetServerMemUsage)) {
        return false;
    }

    if (!LOAD_FUNCTION(byondcore, DMString_Destructor)) {
        return false;
    }

    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!LoadFunctions()) {
            Byond_CRASH("Failed to load byondcore.dll functions");
        }
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" BYOND_EXPORT
CByondValue PerfMark(u4c n, CByondValue v[])
{
    CByondValue ret;
    ByondValue_Clear(&ret);

    if (n != 0) {
        Byond_CRASH("Invalid number of arguments passed, expected 0");
    }

    LARGE_INTEGER pc;
    QueryPerformanceCounter(&pc);

    CByondValue idx;
    CByondValue val;
    ByondValue_Clear(&idx);
    ByondValue_Clear(&val);

    Byond_CreateListLen(&ret, 2);

    ByondValue_SetNum(&idx, 1);
    ByondValue_SetNum(&val, *(float*)&pc.LowPart);
    Byond_WriteListIndex(&ret, &idx, &val);

    ByondValue_SetNum(&idx, 2);
    ByondValue_SetNum(&val, *(float*)&pc.HighPart);
    Byond_WriteListIndex(&ret, &idx, &val);

    return ret;
}

extern "C" BYOND_EXPORT
CByondValue PerfElapsed(u4c n, CByondValue v[])
{
    CByondValue ret;
    ByondValue_Clear(&ret);

    if (n != 3) {
        Byond_CRASH("Invalid number of arguments passed, expected 3");
    }

    if (!ByondValue_IsList(&v[0])) {
        Byond_CRASH("Invalid argument 1 type, LIST expected");
    }

    if (!ByondValue_IsList(&v[1])) {
        Byond_CRASH("Invalid argument 2 type, LIST expected");
    }

    if (!ByondValue_IsNum(&v[2])) {
        Byond_CRASH("Invalid argument 3 type, NUMBER expected");
    }

    CByondValue start = v[0];
    CByondValue end = v[1];
    float scale = ByondValue_GetNum(&v[2]);

    LARGE_INTEGER pcStart = {};
    LARGE_INTEGER pcEnd = {};

    CByondValue idx;
    CByondValue val;
    float tmp;

    ByondValue_SetNum(&idx, 1);
    Byond_ReadListIndex(&start, &idx, &val);
    tmp = ByondValue_GetNum(&val);
    pcStart.LowPart = *(DWORD*)&tmp;

    ByondValue_SetNum(&idx, 2);
    Byond_ReadListIndex(&start, &idx, &val);
    tmp = ByondValue_GetNum(&val);
    pcStart.HighPart = *(DWORD*)&tmp;

    ByondValue_SetNum(&idx, 1);
    Byond_ReadListIndex(&end, &idx, &val);
    tmp = ByondValue_GetNum(&val);
    pcEnd.LowPart = *(DWORD*)&tmp;

    ByondValue_SetNum(&idx, 2);
    Byond_ReadListIndex(&end, &idx, &val);
    tmp = ByondValue_GetNum(&val);
    pcEnd.HighPart = *(DWORD*)&tmp;

    LARGE_INTEGER pcFreq;
    QueryPerformanceFrequency(&pcFreq);

    double elapsed = (pcEnd.QuadPart - pcStart.QuadPart) * scale / (double)pcFreq.QuadPart;

    ByondValue_SetNum(&ret, (float)elapsed);
    return ret;
}

extern "C" BYOND_EXPORT
CByondValue MemoryStats(u4c n, CByondValue v[])
{
    // This function gets memory usage of Byond Server as if you pressed
    // Ctrl+M in dream daemon or typed "memory" in console dd
    // 
    // It calls internal "DungServer::GetServerMemUsage()" and returns a string
    // This is not official or approved api, this is just a hack
    // so this may leak memory, break, crash game. Use at your own risk.

    CByondValue ret;
    ByondValue_Clear(&ret);

    if (n != 0) {
        Byond_CRASH("Invalid number of arguments passed, expected 0");
    }

    DMString mem_usage_str;
    DungServer_GetServerMemUsage(nullptr, &mem_usage_str);
    ByondValue_SetStr(&ret, (char*)mem_usage_str);
    DMString_Destructor(&mem_usage_str);
    return ret;
}
