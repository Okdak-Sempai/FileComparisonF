#ifndef FILECOMPARISON_FILESFUNCSLU_H
#define FILECOMPARISON_FILESFUNCSLU_H

#include <sys/stat.h>

char* selectFilePath();
off_t getFileSize(const char* filePath);
int compareFiles(const char* file1, const char* file2);
int compareFilesCluster(const char* file1, const char* file2);

#endif //FILECOMPARISON_FILESFUNCSLU_H