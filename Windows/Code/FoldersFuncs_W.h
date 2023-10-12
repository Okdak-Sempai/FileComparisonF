#ifndef FOLDER_FUNCS_W_H
#define FOLDER_FUNCS_W_H

#include <Windows.h>

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
WCHAR* selectPathFolder(WCHAR* strPath);
void openFolderInExplorer(const WCHAR* rootPath);
int countFilesInFolder(const WCHAR* folderPath);
int countFilesInFolderANDSUBS(const WCHAR* folderPath);
WCHAR* currentDirectory();
int createFolder(const WCHAR* foldername, const WCHAR* path);
int moveToFolder(WCHAR** folderAddress, WCHAR* FolderToMove);
int folderExists(const WCHAR* folderPath, const WCHAR* folderName);
WCHAR* finalPathFileExplorer(const WCHAR* strPath);
void fileExplorer_list(WCHAR* filepathInit);
void fileExplorer_listSubsFoldersIncluded(WCHAR* filepathInit, WCHAR* star_filepathInit, WCHAR** ListOfFiles, int sizeOfListOfFiles, int* i);
int compareWCHARStrings(const void* a, const void* b);
void sortWCHARStringArray(WCHAR** array, size_t arraySize);

#endif