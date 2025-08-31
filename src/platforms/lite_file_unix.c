#include "lite_file.h"

#include <stdio.h>
#include <stdint.h>

#include <unistd.h>
#include <sys/stat.h>


uint64_t lite_file_write_time(LiteStringView string)
{
    struct stat sb;
    lstat(string.buffer, &sb);

    return (uint64_t)sb.st_mtimespec.tv_sec + (uint64_t)sb.st_mtimespec.tv_nsec / 1000000000;
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


bool lite_create_directory_recursive(LiteStringView path)
{
    bool result = false;

    for (int32_t i = 0, j = 0, n = path.length; i < n; i++)
    {
        const char c = path.buffer[i];
        if (c == '/' || c == '\\')
        {
            if (i - j > 0)
            {
                const LiteStringView directory_path = {
                    .buffer = path.buffer,
                    .length = i + 1,
                };

                mode_t permissions = S_IRWXU | S_IRWXG | S_IRWXO;
                result = mkdir(directory_path.buffer, permissions);
            }

            j = i;
        }
    }

    return result;
}


LiteStringView lite_parent_directory(LiteStringView path)
{
    const char last_char = path.buffer[path.length - 1];
    if (last_char == '/' || last_char == '\\')
    {
        path.length -= 1;
    }

    const char last_index_of_sep_0 = lite_last_index_of_char(path, '\\');
    if (last_index_of_sep_0 != -1)
    {
        path.length = last_index_of_sep_0 + 1;
        return path;
    }

    const char last_index_of_sep_1 = lite_last_index_of_char(path, '/');
    if (last_index_of_sep_1 != -1)
    {
        path.length = last_index_of_sep_1 + 1;
        return path;
    }

    return lite_string_lit("");
}

//! EOF

