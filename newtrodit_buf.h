#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

DWORD GetNumCharsInConsoleBuffer()
{
    CONSOLE_SCREEN_BUFFER_INFO buffer_info = {0};
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buffer_info) != FALSE)
        return (DWORD)((buffer_info.dwSize.X * (buffer_info.dwCursorPosition.Y + 1)) - (buffer_info.dwSize.X - (buffer_info.dwCursorPosition.X)));
    else
        return 0;
}

DWORD ReadConsoleBuffer(char *buffer, DWORD bufsize)
{
    DWORD num_character_read = 0;
    COORD first_char_to_read = {0};
    if (ReadConsoleOutputCharacterA(GetStdHandle(STD_OUTPUT_HANDLE), buffer, bufsize, first_char_to_read, &num_character_read) != FALSE)
        buffer[bufsize - 1] = '\0';
    else
        buffer[0] = '\0';

    return num_character_read;
}

char* RestoreBuffer()
{
    DWORD bufsize = GetNumCharsInConsoleBuffer();
    if (bufsize > 0)
    {
        bufsize++; // Add 1 for zero-ending char

        char *buffer = malloc(bufsize);
        memset(buffer, 0, bufsize);

        ReadConsoleBuffer(buffer, bufsize);
        return buffer;
    }
    
}