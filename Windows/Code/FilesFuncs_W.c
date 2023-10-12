#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <commdlg.h>

DWORD getFileSize(const WCHAR* filePath)
/*
* Includes:
*   windows.h [HANDLE; INVALID_HANDLE_VALUE; CreateFile(); GENERIC_READ; FILE_SHARE_READ; OPEN_EXISTING; FILE_ATTRIBUTE_NORMAL; GetFileSize(); CloseHandle; DWORD]
* Params:
*   WCHAR* filePath = The path of a file
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

WCHAR* selectFilePath()
/*
* Includes: 
*   windows.h [OPENFILENAME]
*   wchar.h [WCHAR; wcslen(); wcscpy_s()]
*   commdlg.h [GetOpenFileName()]
*   stdlib.h [malloc]
* Params:
*   None
* Return:
*   WCHAR* => The path of the file choosen by the user
* Description:
*   This function open the file explorer for windows and let the user choice a file, the path of the file is returned
* Notes:
*   None
*/
{
    OPENFILENAME ofn = { 0 };
    WCHAR selectedFilePath[MAX_PATH] = { 0 };

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // Main window
    ofn.lpstrFile = selectedFilePath; // Buffer for path
    ofn.nMaxFile = sizeof(selectedFilePath) / sizeof(selectedFilePath[0]);
    ofn.lpstrTitle = L"Select a file"; // Title of the box
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // Options

    if (GetOpenFileName(&ofn))
    {
        WCHAR* selectedFilePathHeap = (WCHAR*)malloc((wcslen(selectedFilePath) + 1) * sizeof(WCHAR));
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

int compareFiles(const WCHAR* file1, const WCHAR* file2)
/*
* Includes:
*   windows.h [DWORD]
*   stdio.h [wprintf(); FILE; perror(); _wfopen_s(); fgetc(); fclose()]
*   wchar.h
* 
*   FilesFuncs_W.h/.c{getFileSize()}
* Params:
*   const WCHAR* file1 = The path of the 1st file to compare
*   const WCHAR* file2 = The path of the 2nd file to compare
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

int compareFilesCluster(const WCHAR* file1, const WCHAR* file2)
/*
* Includes:
*   windows.h [DWORD]
*   stdio.h [wprintf(); FILE; fclose(); fread()]
*   wchar.h
* 
*   FilesFuncs_W.h/.c{getFileSize()}
* Params:
*   const WCHAR* file1 = The path of the 1st file to compare
*   const WCHAR* file2 = The path of the 2nd file to compare
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

    return 1; // Files are the same.
}

const WCHAR* GetFileExtension(const WCHAR* filePath)
/*
* Includes:
*   windows.h [WCHAR]
*   wchar.h [wcsrchr()]
* Params:
*   const WCHAR* filePath = The path of the file
* Return:
*  const WCHAR* => Return the extension of the file if it exists. If there's none found, returns NULL 
* Description:
*   This function returns the extension of the file it exists and NULL if none found.
* Notes:
*   None
*/
{
    const WCHAR* lastDot = wcsrchr(filePath, L'.');
    if (lastDot != NULL)
    {
        return lastDot + 1;
    }
    else {
        return NULL; // No extension founded
    }
}

WCHAR* GetLastModifiedDate(const WCHAR* filePath)
/*
* Includes:
*   windows.h [GetFileAttributesEx(); FILETIME; FileTimeToSystemTime(); SYSTEMTIME]
*   stdlib.h [malloc]
*   stdio.h [wprintf(); swprintf()]
* Params:
*   const WCHAR* filePath = The path of the file
* Return:
*  const WCHAR* => Return the last modification date of the file if it exists. If there's none found, returns NULL
* Description:
*   This function returns the last modification date of the file it exists or L"NOT ACCESSIBLE" if.. not accessible. As per "YYYY-MM-DD HH:MM:SS" format
* Notes:
*   None
*/
{
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;

    if (GetFileAttributesEx(filePath, GetFileExInfoStandard, &fileInfo)) {
        FILETIME lastModifiedTime = fileInfo.ftLastWriteTime;

        SYSTEMTIME sysTime;
        FileTimeToSystemTime(&lastModifiedTime, &sysTime);

        // String format
        WCHAR* result = (WCHAR*)malloc(20 * sizeof(WCHAR)); // As per "YYYY-MM-DD HH:MM:SS" format
        if (result != NULL) {
            swprintf(result, 20, L"%04d-%02d-%02d %02d:%02d:%02d",
                sysTime.wYear, sysTime.wMonth, sysTime.wDay,
                sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
        }

        return result;
    }
    else {
        wprintf(L"\nImpossible get the file's informations.\n");
        return L"NOT ACCESSIBLE";
    }
}

WCHAR* readLineNumber(FILE* file, int lineNumber)
/*
* Includes:
*   windows.h [WCHAR]
*   stdlib.h
*   stdio.h [fgetws()]
*   wchar.h [WCHAR; _wcsdup()]
* Params:
*   FILE* file = The file, already opened as "r"
*   int lineNumber = Number of the line to read (First line is 1 and not 0)
* Return:
*  WCHAR* => Return the content of the line
* Description:
*   This function returns the content of the lineNumber in file, the file has to be already opened when passed to the function(As "r")
* Notes:
*   /!\ the return has to be free() because of the _wcsdup() /!\ 
*/
{
    WCHAR buffer[4096]; // Assuming a line is 255 characters long
    int currentLine = 0;

    while (currentLine < lineNumber && fgetws(buffer, sizeof(buffer) / sizeof(buffer[0]), file) != NULL)
    {
        currentLine++;
    }

    if (currentLine == lineNumber)
    {
        // Return a dynamically allocated copy of the read line
        return _wcsdup(buffer);
    }
    else
    {
        return NULL; // The line couldn't be read
    }
}

int writeLineNumber(FILE* file, int lineNumber, const WCHAR* content)
/*
* Includes:
*   windows.h [WCHAR]
*   stdio.h [fputws(); fgetws(); fseek()]
*   wchar.h [WCHAR]
* Params:
*   FILE* file = The file, already opened as "w"(only one write) or "a"(to write on next line)
*   int lineNumber = Number of the line to read (First line is 1 and not 0)
*   const WCHAR* content = The content to write on the line
* Return:
*  int 
*       -1 => Error when moving the cursor
*       -2 => Couldn't reach the wanted line
*       0 => Sucess in writting on the line
* Description:
*   This function write the content to the line of the file in argument, the file has to be already opened when passed to the function (As "w" or "a") and returns 0 for sucess or -1 or -2 for failure
* Notes:
*   To write on other than first line you'd need to open(Beforehand) the file as "r"
*/
{
    // Move the cursor at the beginning
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        return -1; // Error
    }

    // Write the content to the preceding lines
    WCHAR buffer[4096]; // Assuming a line is 255 characters long
    int currentLine = 0;
    while (currentLine < lineNumber - 1 && fgetws(buffer, sizeof(buffer) / sizeof(buffer[0]), file) != NULL)
    {
        currentLine++;
    }

    if (currentLine != lineNumber - 1)
    {
        return -2; // Line couldn't be reached
    }

    // Write content on the wanted line
    fputws(content, file);

    // Copy the remaing content of the file
    while (fgetws(buffer, sizeof(buffer) / sizeof(buffer[0]), file) != NULL)
    {
        fputws(buffer, file);
    }

    return 0; // Sucess
}

WCHAR* getFilename(const WCHAR* filepath)
/*
* Includes:
*   windows.h [wcsrchr]
*   wchar.h [WCHAR]
* Params:
*   const WCHAR* filepath = The path of the file you want to extract the name
* Return:
*   WCHAR* => The name of the file from of the filepath, extension included
* Description:
*   The function returns the name of a file, extension included
*   Ex: filepath: L"\\Z:\\Documents\\steve.png" then returns L"steve.png"
* Notes:
*   Include the extension in the name
*/
{
    const WCHAR* filename = wcsrchr(filepath, L'\\');
    if (filename != NULL)
    {

        filename++;  // Move past the '\\' character
    }
    else
    {
        return filepath;  // Use the whole path if '\\' not found
    }
}

WCHAR* createFileInDirectory(const WCHAR* directoryPath, const WCHAR* fileName)
/*
* Includes:
*   windows.h [WCHAR]
*   wchar.h [wcscpy_s(); wcslen(); wcscat_s(); _wcsdup()]
*   stdio.h [_wfopen_s(); fclose()] 
* Params:
*   const WCHAR* directoryPath = The path where to create file
*   const WCHAR* fileName = The name of the file as a txt
* Return:
*   WCHAR* => The path of the file created 
* Description:
*   This function creates a file with the extension .txt and the fileName to directoryPath folder.
* Notes:
*   This creates a .txt file
*/
{
    WCHAR filePath[MAX_PATH];
    wcscpy_s(filePath, MAX_PATH, directoryPath);

    if (wcslen(directoryPath) > 0 && directoryPath[wcslen(directoryPath) - 1] != L'\\')
    {
        wcscat_s(filePath, MAX_PATH, L"\\");
    }

    wcscat_s(filePath, MAX_PATH, fileName);
    wcscat_s(filePath, MAX_PATH, L".txt");

    FILE* file;
    if (_wfopen_s(&file, filePath, L"w") == 0)
    {
        fclose(file);
        return _wcsdup(filePath);
    }
    else
    {
        wprintf(L"Failed to create the file: %ls\n", filePath);
        return NULL;
    }
}

int writeLineRegister(FILE* file, const WCHAR* content)
/*
* Includes:
*   stdio.h [fseek(); fputwc(); fputws()]
*   wchar.h [wcslen()]
* Params:
*   FILE* file = The file, already opened as "a"(to write on next line)
*   const WCHAR* content = The content to write on the line
* Return:
*  int
*       -1 => Error when moving the cursor
*       0 => Sucess in writting on the line
* Description:
*   This function write the content to the next line of the file in argument, the file has to be already opened when passed to the function (As "a") and returns 0 for sucess or -1 for failure
*   This function is intended to be used for the Register file of the FBDFuncs library
* Notes:
*   To write on other than first line you'd need to open(Beforehand) the file as "r"
*/
{
    // Move the cursor at the end of the file
    if (fseek(file, 0, SEEK_END) != 0)
    {
        return -1; // Error
    }

    // add a new line if not already done
    int contentLength = wcslen(content);
    if (contentLength > 0 && content[contentLength - 1] != L'\n')
    {
        fputwc(L'\n', file);
    }

    // Write the content
    fputws(content, file);

    return 0; // Sucess
}