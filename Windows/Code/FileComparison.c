#include <stdio.h>
#include <windows.h>

#include "FilesFuncs_W.h"
#include "FoldersFuncs_W.h"
#include "Tools_W.h"

int main() 
{
    wprintf(L"==========================================\n");
    wprintf(L"/!\\ WARNING: Only use it to compare folders with the same number of files and the same names for files to be compared. (In each folders.) /!\\ \n");
    wprintf(L"Hello!\t Please select which mode to use:\n");
    wprintf(L"Press 1\tfor the faster one that uses Clusters of 4096 bytes.\n");
    wprintf(L"Press 2\tfor the slower but more accurate method that compares literally byte per byte.\n\n");
    wprintf(L"choice: ");
    unsigned int choiceM = 0;
    int scanfResult;
    WCHAR* star_FirstFolder = NULL;
    WCHAR* star_SecondFolder = NULL;
    WCHAR** FirstFolderList = NULL;
    WCHAR** SecondFolderList = NULL;
    WCHAR** SimilarList = NULL;
    WCHAR** DifferentList = NULL;
    WCHAR** DifferentcmpStatus = NULL;
    WCHAR* currentPath = NULL;
    WCHAR* filename = NULL;
    int FirstFolderpathArraySize = 0;
    int SecondFolderpathArraySize = 0;
    int result = 0;

    do
    {
        scanfResult = scanf_s("%d%*c", &choiceM);
        if (scanfResult != 1 || (choiceM != 1 && choiceM != 2))
        {
            wprintf(L"Only 1 or 2.\tchoice: ");
            while (getchar() != '\n');
        }
    } while (choiceM != 1 && choiceM != 2);

    do 
    {
        wprintf(L"==========================================\n");
    //1st Folder
    wprintf(L"Please Select the 1st folder to compare.\n");

    WCHAR* FirstFolder = selectPathFolder(L"1st comparison");
    if (FirstFolder == NULL)
    {
        free(FirstFolder);
        return -1;
    }
    WCHAR* star_FirstFolder = finalPathFileExplorer(FirstFolder);

    int FirstFolderpathArraySize = countFilesInFolderANDSUBS(FirstFolder);


    // Declare a list of pointer to WCHAR* [List of files paths]
    FirstFolderList = (WCHAR**)calloc(FirstFolderpathArraySize, sizeof(WCHAR*));
    if (FirstFolderList == NULL)
    {
        free(FirstFolderList);
        return -1;
    }

    int FirstFolderCount = 0;
    fileExplorer_listSubsFoldersIncluded(FirstFolder, star_FirstFolder, FirstFolderList, FirstFolderpathArraySize, &FirstFolderCount);

    //Sorting

    sortWCHARStringArray(FirstFolderList, (size_t)FirstFolderpathArraySize);

    //Similar list

    SimilarList = (WCHAR**)calloc(FirstFolderpathArraySize, sizeof(WCHAR*));
    if (SimilarList == NULL)
    {
        free(SimilarList);
        return -1;
    }

    //Differents list

    DifferentList = (WCHAR**)calloc(FirstFolderpathArraySize, sizeof(WCHAR*));
    if (DifferentList == NULL)
    {
        free(DifferentList);
        return -1;
    }
    DifferentcmpStatus = (WCHAR**)calloc(FirstFolderpathArraySize, sizeof(WCHAR*));
    if (DifferentcmpStatus == NULL)
    {
        free(DifferentcmpStatus);
        return -1;
    }

    //2nd Folder

    wprintf(L"Please Select the 2nd folder to compare.\n");

    WCHAR* SecondFolder = selectPathFolder(L"2nd comparison");
    if (SecondFolder == NULL)
    {
        free(SecondFolder);
        return -1;
    }
    WCHAR* star_SecondFolder = finalPathFileExplorer(SecondFolder);

    int SecondFolderpathArraySize = countFilesInFolderANDSUBS(SecondFolder);


    // Declare a list of pointer to WCHAR* [List of files paths]
    SecondFolderList = (WCHAR**)calloc(SecondFolderpathArraySize, sizeof(WCHAR*));
    if (SecondFolderList == NULL)
    {
        free(SecondFolderList);
        return -1;
    }

    int SecondFolderCount = 0;
    fileExplorer_listSubsFoldersIncluded(SecondFolder, star_SecondFolder, SecondFolderList, SecondFolderpathArraySize, &SecondFolderCount);

    //Sorting

    sortWCHARStringArray(SecondFolderList, (size_t)SecondFolderpathArraySize);

    //////

    result = 0;
    LARGE_INTEGER startTime, endTime, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startTime);

    int iSimilarList = 0;
    int iDifferentList = 0;
    for (int j = 0; j < FirstFolderpathArraySize; j++)
    {
        wprintf(L"\nInitating the comparison for %ls and %ls\n", FirstFolderList[j], SecondFolderList[j]);

        switch ((choiceM == 2) ? compareFiles(FirstFolderList[j], SecondFolderList[j]) : compareFilesCluster(FirstFolderList[j], SecondFolderList[j]))
        {
        case -1:
            wprintf(L"The files are different.\n");
            DifferentList[iDifferentList] = FirstFolderList[j];
            DifferentcmpStatus[iDifferentList] = L"The files are different.";
            iDifferentList++;
            break;
        case 1:
            wprintf(L"The files are identical.\n");
            SimilarList[iSimilarList] = FirstFolderList[j];
            iSimilarList++;
            result++;
            break;
        case -2:
            wprintf(L"The file 2 is greater than the file 1.\n");
            DifferentList[iDifferentList] = FirstFolderList[j];
            DifferentcmpStatus[iDifferentList] = L"The file 2 is greater than the file 1.";
            iDifferentList++;
            break;
        case 2:
            wprintf(L"The file 1 is greater than the file 2.\n");
            DifferentList[iDifferentList] = FirstFolderList[j];
            DifferentcmpStatus[iDifferentList] = L"The file 1 is greater than the file 2.";
            iDifferentList++;
            break;
        default:
            wprintf(L"Unknown result.\n");
            DifferentList[iDifferentList] = FirstFolderList[j];
            DifferentcmpStatus[iDifferentList] = L"Unknown result.";
            iDifferentList++;
            break;
        }
    }
    if (result == FirstFolderpathArraySize)
    {
        wprintf(L"\n\tThe folders are identical! [%d/%d Identical] \n",result, FirstFolderpathArraySize);
    }
    else
    {
        wprintf(L"\n\tThe folders are different! [%d/%d Identical]\n",result, FirstFolderpathArraySize);
    }
    QueryPerformanceCounter(&endTime);
    double elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;
    wprintf(L"Execution time: %.2f millisecondes\n", elapsedTime);

    //Result saving

    //Filename
    filename = getFilename(FirstFolder);
    WCHAR* filenameF = NULL;

    WCHAR* dateBackup = getCurrentDateWCHAR();
    WCHAR* timeBackup = getCurrentTimeWCHAR();
    // Allocate memory for BackupFolderName
    size_t bufferSize = wcslen(L"[FC] ") + wcslen(filename) + wcslen(dateBackup) + wcslen(timeBackup) + wcslen(L" [] ") + 20; // Estimate size
    filenameF = (WCHAR*)malloc(bufferSize * sizeof(WCHAR));
    if (filenameF != NULL)
    {
        swprintf(filenameF, bufferSize, L"[FC] %s [%s_%s]", filename, dateBackup, timeBackup);
        free(dateBackup);
        free(timeBackup);
    }
    else
    {
        wprintf(L"Memory allocation failed.\n");
        free(dateBackup);
        free(timeBackup);
        return -1;
    }

    //Save results to a folder
    WCHAR* savePath = NULL;

    if (choice(L"Do you want to save the result? ", 1, L"Or not?", 0) == 1)
    {
        // Folder path
        if (choice(L"To save in the folder of the software", 1, L"To save to a specific folder", 2) == 1) //Default
        {
            currentPath = currentDirectory();
            if (currentPath == NULL)
            {
                return -1;
            }
            savePath = (WCHAR*)CoTaskMemAlloc((wcslen(currentPath) + 1) * sizeof(WCHAR));
            if (savePath == NULL)
            {
                return -1;
            }

            wcscpy_s(savePath, wcslen(currentPath) + 1, currentPath);

            if (moveToFolder(&savePath, L"[FC] FileComparison") == -1)
            {
                wprintf(L"Critical error");
                return -1;
            }
        }
        else // Specific
        {
            savePath = selectPathFolder(L"result saving");
            if (moveToFolder(&savePath, L"[FC] FileComparison") == -1)
            {
                wprintf(L"Critical error");
                return -1;
            }
        }

        WCHAR* filenameF_path = createFileInDirectory(savePath, filenameF);
        FILE* fpfile = NULL;
        _wfopen_s(&fpfile, filenameF_path, L"a");

        //Results to save
        if (choice(L"Do you want the differents results? ", 1, L"Or the identicals results", 2) == 2) //Identical results
        {
            for (int p = 0; p < FirstFolderpathArraySize; p++)
            {
                if (SimilarList[p] != NULL)
                {
                    writeLineRegister(fpfile, SimilarList[p]);
                    writeLineRegister(fpfile, L"The files are identical.");
                    writeLineRegister(fpfile, L"\n");
                }
            }
        }
        else //Different results
        {
            for (int p = 0; p < FirstFolderpathArraySize; p++)
            {
                if (DifferentList[p] != NULL)
                {
                    writeLineRegister(fpfile, DifferentList[p]);
                    writeLineRegister(fpfile, DifferentcmpStatus[p]);
                    writeLineRegister(fpfile, L"\n");
                }
            }
        }
        fclose(fpfile);
        wprintf(L"File save to %ls", savePath);
        openFolderInExplorer(savePath);
    }

    printf("\nPress 0 to close or Enter (Or anything) to compare other files.\n");

    //All the free
    CoTaskMemFree(FirstFolder); FirstFolder = NULL;
    CoTaskMemFree(SecondFolder); SecondFolder = NULL;

    free(star_FirstFolder); star_FirstFolder = NULL;
    free(star_SecondFolder); star_SecondFolder = NULL;

    for (int i = 0; i < FirstFolderpathArraySize; i++)
    {
        free(FirstFolderList[i]); FirstFolderList[i] = NULL;
    }

    free(FirstFolderList); FirstFolderList = NULL;

    for (int i = 0; i < SecondFolderpathArraySize; i++)
    {
        free(SecondFolderList[i]); SecondFolderList[i] = NULL;
    }

    free(SecondFolderList); SecondFolderList = NULL;

    free(SimilarList); SimilarList = NULL;
    free(DifferentList); DifferentList = NULL;

    free(DifferentcmpStatus); DifferentcmpStatus = NULL;
    CoTaskMemFree(savePath); savePath = NULL;

    } while (getchar() != '0');
    return 0;
}
