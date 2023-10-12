#ifndef FILES_FUNCS_W_H
#define FILES_FUNCS_W_H

#include <windows.h>

DWORD getFileSize(const WCHAR* filePath);
WCHAR* selectFilePath();
int compareFiles(const WCHAR* file1, const WCHAR* file2);
int compareFilesCluster(const WCHAR* file1, const WCHAR* file2);
const WCHAR* GetFileExtension(const WCHAR* filePath);
WCHAR* GetLastModifiedDate(const WCHAR* filePath);
WCHAR* readLineNumber(FILE* file, int lineNumber);
int writeLineNumber(FILE* file, int lineNumber, const WCHAR* content);
WCHAR* getFilename(const WCHAR* filepath);
WCHAR* createFileInDirectory(const WCHAR* directoryPath, const WCHAR* fileName);
int writeLineRegister(FILE* file, const WCHAR* content);

#endif