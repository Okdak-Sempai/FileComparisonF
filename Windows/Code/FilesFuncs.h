#ifndef FILES_FUNCS_H
#define FILES_FUNCS_H

#include <windows.h>

DWORD getFileSize(const wchar_t* filePath);
wchar_t* selectFilePath();
int compareFiles(const wchar_t* file1, const wchar_t* file2);
int compareFilesCluster(const wchar_t* file1, const wchar_t* file2);

#endif