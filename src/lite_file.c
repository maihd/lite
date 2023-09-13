#include "lite_file.h"

#include <stdio.h>

#define WIN32_CLEAN_AND_MEAN
#include <Windows.h>

uint64_t lite_file_write_time(LiteStringView string)
{
    HANDLE hFile = CreateFileA(
        string.buffer, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        // @todo(maihd): handle error
        return 0;
    }

    FILETIME ftWriteTime;
    BOOL     bGetFileTime = GetFileTime(hFile, nullptr, nullptr, &ftWriteTime);
    CloseHandle(hFile);
    if (!bGetFileTime)
    {
        // @todo(maihd): handle error
        return 0;
    }

    ULARGE_INTEGER uiTime = {.LowPart  = ftWriteTime.dwLowDateTime,
                             .HighPart = ftWriteTime.dwHighDateTime};

    return (uint64_t)uiTime.QuadPart;
}

bool lite_is_binary_file(LiteStringView path)
{
    FILE* file = fopen(path.buffer, "rb");
    if (!file)
    {
        return false;
    }

    uint8_t first_4_bytes[128];
    size_t  bytes_read = fread(first_4_bytes, 1, sizeof(first_4_bytes), file);
    fclose(file);

    // Utf8 BOM
    if (first_4_bytes[0] == 0xef && first_4_bytes[1] == 0xbb &&
        first_4_bytes[2] == 0xbf)
    {
        return false;
    }

    // Utf32 BOM
    if (*(uint32_t*)first_4_bytes == 0xfffe0000)
    {
        return false;
    }

    // Utf16 BOM
    if ((first_4_bytes[0] == 0xfe && first_4_bytes[1] == 0xff) ||
        (first_4_bytes[0] == 0xff && first_4_bytes[1] == 0xfe))
    {
        return false;
    }

    // ASCII or binary
    uint8_t* c = first_4_bytes;
    for (size_t i = 0; i < bytes_read; i++)
    {
        if (c[i] < 0x20 && c[i] != '\n' && c[i] != '\t' && c[i] != '\r' &&
            c[i] != '\f')
        {
            return true;
        }
    }

    return false;
}
//! EOF
