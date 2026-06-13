#include <windows.h>
#include <byondapi.h>

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