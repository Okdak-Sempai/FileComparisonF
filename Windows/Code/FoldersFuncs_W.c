#include<windows.h>
#include <shlobj.h>
#include <stddef.h>
#include <wchar.h>

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
/*
* Includes:
*   windows.h[GetLogicalDrives(); SendMessage()]
*   wchar.h[swprintf()]
* Params:
*   HWND hwnd = The handle to the dialog window
*   UINT uMsg = The message being sent
*   LPARAM lParam = Additional message-specific information
*   LPARAM lpData = User-defined data provided during the initialization
* Return:
*   int => Returns 0 if succeed
* Description:
*   This function is a callback procedure used in a file dialog. It initializes the dialog by setting the root folder to the first available drive.
* Notes:
*   It is use and mandatory for the selectPathFolder() function of the FoldersFuncsW library
*/
{
    if (uMsg == BFFM_INITIALIZED)
    {
        // Get the list of avalaible drives
        int drives = GetLogicalDrives();
        WCHAR currentDrive = L'A'; // beginning of the drives letters 

        while (drives)
        {
            if (drives & 1)
            {
                // Root folder for for each drive configuration
                WCHAR rootPath[4];
                swprintf(rootPath, sizeof(rootPath) / sizeof(rootPath[0]), L"%c:\\", currentDrive);
                SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)rootPath);
                break; // Display only the first drive
            }

            drives >>= 1;
            ++currentDrive;
        }
    }

    return 0;
}

WCHAR* selectPathFolder(WCHAR* strPath)
/*
* Includes: 
*   windows.h
*   Shlobj.h [BROWSEINFO; LPITEMIDLIST; SHBrowseForFolder(); SHGetPathFromIDList(); CoTaskMemAlloc(); CoTaskMemFree()]
*   wchar.h [WCHAR; swprintf()]
*   stdio.h [wprintf()]
* Params:
*   WCHAR* strPath = Name of the action needed 
* Return:
*   WCHAR* => Return the path of the Folder selected
* Description:
*   This function opens a dialog box and let the user choose a folder and the function returns the folder, the title of the box can be influenced using the argument
* Notes:
*   /!\ the return has to be CoTaskMemFree() because of the CoTaskMemAlloc() /!\ 
*/
{
    BROWSEINFO browseInfo = { 0 };
    browseInfo.hwndOwner = NULL;
    browseInfo.pidlRoot = NULL;
    browseInfo.pszDisplayName = NULL;
    browseInfo.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
    browseInfo.lpfn = BrowseCallbackProc;
    WCHAR title[MAX_PATH + 50];
    swprintf(title, sizeof(title) / sizeof(title[0]), L"Select the folder path for %s", strPath);
    browseInfo.lpszTitle = title;

    // Display the folder selection dialog box
    LPITEMIDLIST pidlSelectedFolder = SHBrowseForFolder(&browseInfo);

    if (pidlSelectedFolder != NULL)
    {
        WCHAR* selectedFolderPath = (WCHAR*)CoTaskMemAlloc(MAX_PATH * sizeof(WCHAR));
        if (SHGetPathFromIDList(pidlSelectedFolder, selectedFolderPath))
        {
            wprintf(L"The selected folder is: %s\n", selectedFolderPath);
            CoTaskMemFree(pidlSelectedFolder);
            return selectedFolderPath;
        }
        CoTaskMemFree(selectedFolderPath);
        CoTaskMemFree(pidlSelectedFolder);
    }

    return NULL;
}

void openFolderInExplorer(const WCHAR* rootPath)
/*
* Includes:
*   windows.h [ShellExecute(); SW_SHOWNORMAL]
* Params:
*   const WCHAR* rootPath = The path of the folder to open in Windows Explorer
* Return:
*   void
* Description:
*   This function opens the specified folder in Windows Explorer.
* Notes:
*   None
*/
{
    ShellExecute(NULL, L"open", rootPath, NULL, NULL, SW_SHOWNORMAL);
}

int countFilesInFolder(const WCHAR* folderPath)
/*
* Includes:
*   windows.h [FindFirstFile(); FindNextFile(); FindClose();dwFileAttributes; FILE_ATTRIBUTE_DIRECTORY; INVALID_HANDLE_VALUE; WIN32_FIND_DATA]
* Params:
*   const WCHAR* folderPath = The path of the folder to count the number of files in
* Return:
*   int = The number of files in the specified folder
* Description:
*   This function counts the number of files (excluding directories) in the specified folder.
* Notes:
*   None
*/
{
    WCHAR searchPath[MAX_PATH];
    wcscpy_s(searchPath, MAX_PATH, folderPath);
    wcscat_s(searchPath, MAX_PATH, L"\\*.*");

    int itemCount = 0;
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(searchPath, &findData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                itemCount++;
            }
            else if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0)
            {
                // Exclude "." and ".." entries
                itemCount++;
            }
        } while (FindNextFile(hFind, &findData));

        FindClose(hFind);
    }

    return itemCount;
}

int countFilesInFolderANDSUBS(const WCHAR* folderPath)
/*
* Includes:
*   windows.h [FindFirstFile(); FindNextFile(); FindClose();dwFileAttributes; FILE_ATTRIBUTE_DIRECTORY; INVALID_HANDLE_VALUE; WIN32_FIND_DATA]
*   wchar.h [swprintf()]
* Params:
*   const WCHAR* folderPath = The path of the folder to count the number of files in
* Return:
*   int = The number of files in the specified folder(sub files included
* Description:
*   This function counts the number of files (including directories) in the specified folder.
* Notes:
*   None
*/
{
    int itemCount = 0;
    WCHAR searchPath[MAX_PATH];
    WIN32_FIND_DATA findData;
    HANDLE hFind;

    // Create a search path for the current folder
    wcscpy_s(searchPath, MAX_PATH, folderPath);
    wcscat_s(searchPath, MAX_PATH, L"\\*.*");

    hFind = FindFirstFile(searchPath, &findData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                // It's a file, so increment the count
                itemCount++;
            }
            else if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0)
            {
                // It's a subdirectory (exclude "." and ".." entries)
                // Recursively count files in the subdirectory and add to the count
                WCHAR subFolderPath[MAX_PATH];
                swprintf(subFolderPath, MAX_PATH, L"%s\\%s", folderPath, findData.cFileName);
                itemCount += countFilesInFolderANDSUBS(subFolderPath);
            }
        } while (FindNextFile(hFind, &findData));

        FindClose(hFind);
    }

    return itemCount;
}

WCHAR* currentDirectory()
/*
* Includes:
*   windows.h [GetCurrentDirectoryA(); CP_UTF8; MultiByteToWideChar()]
*   stdlib.h
*   stdio.h
*   wchar.h
* Params:
*   None
* Return:
*   WCHAR* => Returns the current working directory as a wide character string (WCHAR*).
* Description:
*   This function retrieves the current working directory and returns it as a wide character string (WCHAR*).
* Notes:
*   /!\ the return has to be free() /!\
*/
{
    CHAR buffer[MAX_PATH];
    DWORD size = GetCurrentDirectoryA(MAX_PATH, buffer); // Utilisation de GetCurrentDirectoryA pour obtenir une chaîne de caractères multi-octets
    if (size == 0)
    {
        wprintf(L"Error getting the working directory.\n");
        return NULL;
    }

    // Conversion de la chaîne de caractères multi-octets en WCHAR
    int wideCharLength = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, NULL, 0);
    if (wideCharLength == 0)
    {
        wprintf(L"Error converting the character string.\n");
        return NULL;
    }

    WCHAR* wideBuffer = (WCHAR*)malloc((wideCharLength + 1) * sizeof(WCHAR));
    if (wideBuffer == NULL)
    {
        wprintf(L"Error allocating memory.\n");
        return NULL;
    }

    if (MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wideBuffer, wideCharLength + 1) == 0)
    {
        wprintf(L"Error converting the character string.\n");
        free(wideBuffer);
        return NULL;
    }

    //wprintf(L"%s", wideBuffer);
    return wideBuffer;
}

int createFolder(const WCHAR* foldername, const WCHAR* path)
/*
* Includes:
*   windows.h [CreateDirectory(); swprintf()]
* Params:
*   const WCHAR* foldername = The name of the folder to create
*   const WCHAR* path = The path where to create the folder
* Return:
*   int
*       -1 => Failure, might be because the file already exists
*       0 => Sucess
* Description:
*   This function create a folder with foldername in the path
* Notes:
*   It just create the folder
*/
{
    // Construire le chemin complet du nouveau dossier
    WCHAR newFolderPath[MAX_PATH];
    swprintf(newFolderPath, MAX_PATH, L"%s\\%s", path, foldername);

    if (CreateDirectory(newFolderPath, NULL))
    {
        return 0; //Sucess
        //wprintf(L"Folder created: %ls\n", newFolderPath);
    }
    else
    {
        return -1; //Failure
        //wprintf(L"Failed to create folder: %ls\n", newFolderPath); // Files already exists then
    }
}

int moveToFolder(WCHAR** folderAddress, WCHAR* FolderToMove)
/*
* Includes:
*   windows.h [wcscat_s; CoTaskMemRealloc]
*   stddef.h [size_t]
*   FoldersFuncs_W.h/c{createFolder()}
* Params:
*   WCHAR** folderAddress = Pointer to the address of the folder where you want to move FolderToMove /!\ This HAS to be an allocated pointor 
*   WCHAR* FolderToMove = The name of the folder to add to folderAddress and create
* Return:
*   int
*       0 => No error
*       -1 => Critical error
* Description:
*   This function moves a folder specified by "FolderToMove" to the location specified by "*folderAddress".
*   It first creates the destination folder within "*folderAddress", and then appends the "FolderToMove" to the "*folderAddress" to construct the final path.
*   The function also reallocates memory for "*folderAddress" to accommodate the longer path.
*   It returns 0 when suceed and -1 if there's an allocation error
* Notes:
*   /!\ The folderAdress HAS to be an allocated pointer /!\
*/
{
    createFolder(FolderToMove, *folderAddress);

    // Calculate the required size for the new path
    size_t newFolderPathSize = wcslen(*folderAddress) + wcslen(FolderToMove) + 2; // +2 for the backslash and null terminator

    // Reallocate memory for the new path
    WCHAR* newFolderPath = (WCHAR*)CoTaskMemRealloc(*folderAddress, newFolderPathSize * sizeof(WCHAR));
    if (newFolderPath == NULL)
    {
        return -1; // Critical error
    }

    if (newFolderPath != NULL)
    {
        // Update folderAddress to point to the new memory
        *folderAddress = newFolderPath;

        // Add "\\" at the end of folderAddress
        wcscat_s(*folderAddress, newFolderPathSize, L"\\");

        // Add FolderToMove to the end
        wcscat_s(*folderAddress, newFolderPathSize, FolderToMove);

        //wprintf(L"The final path is: %s\n", *folderAddress);
        return 0;
    }
    else
    {
        wprintf(L"Error reallocating memory for the new path.\n");
        return -1;
    }
}

int folderExists(const WCHAR* folderPath, const WCHAR* folderName)
/*
* Includes:
*   windows.h [GetFileAttributes(); INVALID_FILE_ATTRIBUTES; FILE_ATTRIBUTE_DIRECTORY]
* Params:
*   const WCHAR* folderPath = The path to the parent folder where you want to check for the existence of folderName.
*   const WCHAR* folderName = The name of the folder you want to check for existence within the L"folderPath".
* Return:
*   int
*       1 => The folder exists within the specified 'folderPath'.
*       0 => The folder does not exist within the specified 'folderPath'.
* Description:
*   This function checks if a folder with the name 'folderName' exists within the directory specified by 'folderPath'.
*   It constructs the full path by combining 'folderPath' and 'folderName' and then uses the GetFileAttributes function to determine if the folder exists.
*   If the folder exists, it returns 1; otherwise, it returns 0.
* Notes:
*   None
*/
{
    WCHAR fullPath[MAX_PATH];
    swprintf(fullPath, MAX_PATH, L"%s\\%s", folderPath, folderName);

    DWORD attributes = GetFileAttributes(fullPath);

    if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        return 1; // Exists
    }
    else
    {
        return 0; // Doesn't exists
    }
}

WCHAR* finalPathFileExplorer(const WCHAR* strPath)
/*
* Includes:
*   WCHAR [WCHAR; size_t; malloc(); wcscpy_s()]
* Params:
*   const WCHAR* strPath = The path to add the L"\\*"
* Return:
*   WCHAR* => Returns the path with the L"\\*"
* Description:
*   This function adds L"\\*" to the strPath WCHAR* and returns the modificated WCHAR*
*   Mandatory for FindFirstFile() and thus the use of fileExplorer_list() and fileExplorer() from FBDFuncs_W.h/.c
*
*    /!\ the return has to be free() because of the malloc() /!\
* Notes:
*    /!\ the return has to be free() because of the malloc() /!\
*/
{
    const WCHAR* addition = L"\\*";
    size_t originalLength = wcslen(strPath);
    size_t additionLength = wcslen(addition);

    // Calculer la longueur totale du nouveau chemin
    size_t newPathLength = originalLength + additionLength + 1; // +1 for null character

    // Allocate memory for new path
    WCHAR* newPath = (WCHAR*)malloc(newPathLength * sizeof(WCHAR));

    // Copy original path to new path
    wcscpy_s(newPath, newPathLength, strPath);

    // Append the addition to the new path
    wcscat_s(newPath, newPathLength, addition);

    return newPath; // Return the new path
}

void fileExplorer_list(WCHAR* filepathInit)
/*
* Includes:
*   windows.h [FindFirstFileW(); FindNextFileW(); FindClose()]
* Params:
*   WCHAR* filepathInit = The initial path for file searching, This path must end with a L"\\*". This is recommended to use a path transformed by finalPathFileExplorer() from FoldersFuncs_W.h/.c
* Return:
*   None
* Description:
*   This function lists and print all the files in a directory specified by 'filepathInit'.
* Notes:
*   This does not go to the subfolders
*/
{
    WIN32_FIND_DATA FileData;
    HANDLE hFind = FindFirstFile(filepathInit, &FileData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            WCHAR fullPath[MAX_PATH];
            swprintf(fullPath, MAX_PATH, L"%s\\%s", filepathInit, FileData.cFileName);
            // Print the path of the folder
            wprintf(L"File: %ls\n", fullPath);
        } while (FindNextFile(hFind, &FileData));
        FindClose(hFind);
    }
    return;
}

void fileExplorer_listSubsFoldersIncluded(WCHAR* filepathInit, WCHAR* star_filepathInit, WCHAR** ListOfFiles, int sizeOfListOfFiles, int* i)
/*
* Includes:
*   stdlib.h [calloc()]
*   windows.h [FindFirstFileW(); FindNextFileW(); FindClose()]
*   wchar.h [swprintf(); wcslen(); wcscpy_s(); wprintf(); wcscmp()]
* Params:
*   WCHAR* filepathInit = Path of the folder to explore
*   WCHAR* star_filepathInit = Path of the folder to explore with L"\\*"
*   WCHAR** ListOfFiles = An array of WCHAR* where we will write all of the adress in the folder
*   int sizeOfListOfFiles = size of ListOfFiles
*   int* i = Pass it initialized at 0, it will be incremented for each file
* Return:
*   None
* Description:
*   This function lists and save to ListOfFiles all the files in a directory specified by 'filepathInit'.
*   This will go in all of the subfolders
* Notes:
*   None
*/
{
    WIN32_FIND_DATA FileData;
    HANDLE hFind = FindFirstFile(star_filepathInit, &FileData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                // It's a file, so print its path
                WCHAR fullPath[MAX_PATH];
                swprintf(fullPath, MAX_PATH, L"%s\\%s", filepathInit, FileData.cFileName);
                //wprintf(L"File: %ls\n", fullPath);
                // Do stuff with files // Here we fill the array with all the path in the filepathInit Folder
                ListOfFiles[*i] = (WCHAR*)calloc((wcslen(fullPath) + 1) , sizeof(WCHAR));
                if (ListOfFiles[*i] != NULL)
                {
                    wcscpy_s(ListOfFiles[*i], wcslen(fullPath) + 1, fullPath);
                }
                else
                {
                    wprintf(L"Memory allocation failed for ListOfFiles[%d].\n", *i);
                }
                //wprintf(L"File:Number %d: %ls\n", *i, ListOfFiles[*i]);
                (*i)++;
            }
            else
            {
                if (wcscmp(FileData.cFileName, L".") != 0 && wcscmp(FileData.cFileName, L"..") != 0)
                {
                    WCHAR subdirectory[MAX_PATH];
                    swprintf(subdirectory, MAX_PATH, L"%s\\%s", filepathInit, FileData.cFileName);

                    WCHAR star_subdirectory[MAX_PATH];
                    swprintf(star_subdirectory, MAX_PATH, L"%s\\*", subdirectory);

                    // Call the function recursively with the subdirectory
                    fileExplorer_listSubsFoldersIncluded(subdirectory, star_subdirectory, ListOfFiles, sizeOfListOfFiles, i);
                }
            }
        } while (FindNextFile(hFind, &FileData));
        FindClose(hFind);
    }
    return;
}

int compareWCHARStrings(const void* a, const void* b)
// Comparison function for qsort 
// Yeah no idea just copy it to use sortWCHARStringArray and don't ask anything
{
    return wcscmp(*(const WCHAR**)a, *(const WCHAR**)b);
}

void sortWCHARStringArray(WCHAR** array, size_t arraySize)
/*
* Includes:
*   stdlib.h
*   wchar.h
* 
*   FoldersFuncs_W.h/.c {compareWCHARStrings()}
* Params:
*   WCHAR** array = The array of WCHAR*
*   size_t arraySize = the size of the array
* Return:
*   None
* Description:
*   This function quicksort an array of WCHAR*
* Notes:
*   Don't ask why just take the compareWCHARStrings() function and it works
*/
{
    qsort(array, arraySize, sizeof(WCHAR*), compareWCHARStrings);
}
