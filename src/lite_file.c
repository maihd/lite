#include "lite_file.h"

#define WIN32_CLEAN_AND_MEAN
#include <Windows.h>

uint64_t lite_file_write_time(LiteStringView string)
{
    HANDLE hFile = CreateFileA(string.buffer, 
                               GENERIC_READ,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               nullptr,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
                               nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        // @todo(maihd): handle error
        return 0;
    }
    
    FILETIME ftWriteTime;
    BOOL bGetFileTime = GetFileTime(hFile, nullptr, nullptr, &ftWriteTime);
    CloseHandle(hFile);
    if (!bGetFileTime)
    {
        // @todo(maihd): handle error
        return 0;
    }
    
    ULARGE_INTEGER uiTime = {
        .LowPart = ftWriteTime.dwLowDateTime,
        .HighPart = ftWriteTime.dwHighDateTime
    };
    
    return (uint64_t)uiTime.QuadPart;
}

//! EOF
