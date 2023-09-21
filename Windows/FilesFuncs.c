#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

DWORD getFileSize(const wchar_t* filePath)
/*
* Includes:
*   windows.h
* params:
*   wchar_t* filePath = The path of a file
* Return:
*   DWORD => Size of the filePath file
* Description:
*   This function returns the size of a file
* Notes:
*   None
*/
{
    HANDLE hFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) 
    {
        return -1;
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    CloseHandle(hFile);
    return fileSize;
}

wchar_t* selectFilePath()
/*
* Includes: windows.h
* params:
*   None
* Return:
*   wchar_t* => The path of the file choosen by the user
* Description:
*   This function open the file explorer for windows and let the user choice a file, the path of the file is returned
* Notes:
*   None
*/
{
    OPENFILENAME ofn = { 0 };
    wchar_t selectedFilePath[MAX_PATH] = { 0 };

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // Main window
    ofn.lpstrFile = selectedFilePath; // Buffer for path
    ofn.nMaxFile = sizeof(selectedFilePath) / sizeof(selectedFilePath[0]);
    ofn.lpstrTitle = L"Select a file"; // Title of the box
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // Options

    if (GetOpenFileName(&ofn)) 
    {
        wchar_t* selectedFilePathHeap = (wchar_t*)malloc((wcslen(selectedFilePath) + 1) * sizeof(wchar_t));
        if (selectedFilePathHeap != NULL) 
        {
            // Move path
            wcscpy_s(selectedFilePathHeap, wcslen(selectedFilePath) + 1, selectedFilePath);
            return selectedFilePathHeap;
        }
        else {
            wprintf(L"Memory allocation failed.\n");
        }
    }

    return NULL;
}

int compareFiles(const wchar_t* file1, const wchar_t* file2)
/*
* Includes:
*   stdio.h
*   wchar.h
* params:
*   const wchar_t* file1 = The path of the 1st file to compare
*   const wchar_t* file2 = The path of the 2nd file to compare
* Return:
*   int
*       -1 => if the files are differents or if there's an error opening the file
*       -2 => if there's an OEF of file 1 before file 2
*       2 => if there's an OEF of file 2 before file 1
*       1 => if the files are identical
* Description:
*   This function compares file1 and file2 byte per byte and return the result as an int.
* Notes:
*   It includes a progression bar
*/
{
    DWORD file1Size = getFileSize(file1);
    DWORD file2Size = getFileSize(file2);

    DWORD mainfile = (file1Size > file2Size) ? file1Size : file2Size;
    wprintf(L"File 1 size: %lu\n", file1Size);
    wprintf(L"File 2 size: %lu\n", file2Size);
    if (file1Size != file2Size)
    {
        wprintf(L"Files size aren't even alike.");
        return -1;
    }
    FILE* fptr1;
    if (_wfopen_s(&fptr1, file1, L"rb") != 0) 
    {
        perror("Error opening file 1");
        return -1;
    }

    FILE* fptr2;
    if (_wfopen_s(&fptr2, file2, L"rb") != 0) 
    {
        perror("Error opening file 2");
        fclose(fptr1);
        return -1;
    }

    int byte1, byte2;
    int position = 1;
    while (1) {
        byte1 = fgetc(fptr1);
        byte2 = fgetc(fptr2);

        if (byte1 == EOF && byte2 == EOF)
        {
            fclose(fptr1);
            fclose(fptr2);
            wprintf(L"\n");
            return 1; // Files are identical.
        }

        if (byte1 == EOF || byte2 == EOF) 
        {
            fclose(fptr1);
            fclose(fptr2);
            if (byte1 == EOF) 
            {
                wprintf(L"\n");
                return -2; // OEF of file 1 before 2.
            }
            else 
            {
                wprintf(L"\n");
                return 2; // OEF of file 2 before 1.
            }
        }

        if (byte1 != byte2) 
        {
            fclose(fptr1);
            fclose(fptr2);
            wprintf(L"\n");
            return -1; // Files are different.
        }

        position++;
        wprintf(L"\rProgression: %d%%", (int)(((float)position / mainfile) * 100));
    }
}

int compareFilesCluster(const wchar_t* file1, const wchar_t* file2)
/*
* Includes:
*   stdio.h
*   wchar.h
* params:
*   const wchar_t* file1 = The path of the 1st file to compare
*   const wchar_t* file2 = The path of the 2nd file to compare
* Return:
*   int
*       -1 => if the files are differents or if there's an error opening the file
*       1 => if the files are identical
* Description:
*   This function compares file1 and file2 by batch of 4096 byte and return the result as an int.
* Notes:
*   It includes a progression bar
*/
{
    DWORD file1Size = getFileSize(file1);
    DWORD file2Size = getFileSize(file2);

    DWORD mainfile = (file1Size > file2Size) ? file1Size : file2Size;
    wprintf(L"File 1 size: %lu\n", file1Size);
    wprintf(L"File 2 size: %lu\n", file2Size);
    if (file1Size != file2Size)
    {
        wprintf(L"Files size aren't event alike.\n");
        return -1;
    }
    FILE* fptr1;
    if (_wfopen_s(&fptr1, file1, L"rb") != 0) {
        perror("Error opening file 1");
        return -1;
    }

    FILE* fptr2;
    if (_wfopen_s(&fptr2, file2, L"rb") != 0) {
        perror("Error opening file 2");
        fclose(fptr1);
        return -1;
    }

    unsigned char buffer1[4096];
    unsigned char buffer2[4096];

    int bytesRead1, bytesRead2;
    int position = 0;
    while (1) {
        bytesRead1 = fread(buffer1, sizeof(unsigned char), 4096, fptr1);
        bytesRead2 = fread(buffer2, sizeof(unsigned char), 4096, fptr2);

        if (bytesRead1 != bytesRead2 || memcmp(buffer1, buffer2, bytesRead1) != 0) {
            fclose(fptr1);
            fclose(fptr2);
            wprintf(L"\n");
            return -1; // Files are different.
        }

        position += bytesRead1;
        wprintf(L"\rProgression: %d%%", (int)(((float)position / mainfile) * 100));
        if (bytesRead1 < 4096) {
            wprintf(L"\n");
            break;
        }
    }

    fclose(fptr1);
    fclose(fptr2);

    return 1; // Files a the same.
}
