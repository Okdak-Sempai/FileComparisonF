#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <shlobj.h>
#include <Shellapi.h>
#include <wchar.h>

DWORD getFileSize(const wchar_t* filePath) {
    HANDLE hFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return -1;
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    CloseHandle(hFile);
    return fileSize;
}

wchar_t* selectFilePath()
{
    OPENFILENAME ofn = { 0 };
    wchar_t selectedFilePath[MAX_PATH] = { 0 };

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // La fenêtre parent, NULL signifie fenêtre principale
    ofn.lpstrFile = selectedFilePath; // Buffer pour stocker le chemin sélectionné
    ofn.nMaxFile = sizeof(selectedFilePath) / sizeof(selectedFilePath[0]);
    ofn.lpstrTitle = L"Select a file"; // Le titre de la boîte de dialogue
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // Options de la boîte de dialogue

    if (GetOpenFileName(&ofn)) {
        // Allouer la mémoire pour stocker le chemin sélectionné sur le tas (heap)
        wchar_t* selectedFilePathHeap = (wchar_t*)malloc((wcslen(selectedFilePath) + 1) * sizeof(wchar_t));
        if (selectedFilePathHeap != NULL) {
            // Copier le chemin sélectionné dans la mémoire allouée sur le tas
            wcscpy_s(selectedFilePathHeap, wcslen(selectedFilePath) + 1, selectedFilePath);
            return selectedFilePathHeap; // Retourner le pointeur sur la mémoire allouée sur le tas
        }
        else {
            wprintf(L"Memory allocation failed.\n");
        }
    }

    return NULL;
}

int compareFiles(const wchar_t* file1, const wchar_t* file2) {
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

    int byte1, byte2;
    int position = 1;
    while (1) {
        byte1 = fgetc(fptr1);
        byte2 = fgetc(fptr2);

        if (byte1 == EOF && byte2 == EOF) {
            fclose(fptr1);
            fclose(fptr2);
            wprintf(L"\n");
            return 1; // Files are identical.
        }

        if (byte1 == EOF || byte2 == EOF) {
            fclose(fptr1);
            fclose(fptr2);
            if (byte1 == EOF) {
                wprintf(L"\n");
                return -2; // OEF of file 1 before 2.
            }
            else {
                wprintf(L"\n");
                return 2; // OEF of file 2 before 1.
            }
        }

        if (byte1 != byte2) {
            fclose(fptr1);
            fclose(fptr2);
            wprintf(L"\n");
            return -1; // Files are different.
        }

        position++;
        wprintf(L"\rProgression: %d%%", (int)(((float)position/mainfile) * 100));
    }
}

int compareFilesCluster(const wchar_t* file1, const wchar_t* file2) {
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

int main() {

    wprintf(L"Hello!\t Please select which mode to use:\n");
    wprintf(L"Press 1\tfor the faster one that uses Clusters of 4096 bytes.\n");
    wprintf(L"Press 2\tfor the slower but more accurate method that compares literally byte per byte.\n\n");
    wprintf(L"Choice: ");
    unsigned int choice=0;
    int scanfResult;

    do
    {
        scanfResult = scanf_s("%d%*c", &choice);
        if (scanfResult != 1 || (choice != 1 && choice != 2))
        {
            wprintf(L"Only 1 or 2.\tChoice: ");
            while (getchar() != '\n');
        }
    } while (choice != 1 && choice != 2);

    wprintf(L"Please Select the 1st file to compare.\n");

    WCHAR* FirstFile = selectFilePath();
    if (FirstFile == NULL)
    {
        free(FirstFile);
        return -1;
    }
    wprintf(L"The selected file is: %s\n\n", FirstFile);

    wprintf(L"Please Select the 2nd file to compare.\n");

    WCHAR* SecondFile = selectFilePath();
    if (SecondFile == NULL)
    {
        free(SecondFile);
        return -1;
    }
    wprintf(L"The selected file is: %s\n\n", SecondFile);
    
    //////

    wprintf(L"Initating the comparison..\n");
    LARGE_INTEGER startTime, endTime, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startTime);
    switch ((choice == 2) ? compareFiles(FirstFile, SecondFile) : compareFilesCluster(FirstFile, SecondFile))
    {
    case -1:
        wprintf(L"The files are different.\n");
        break;
    case 1:
        wprintf(L"The files are identical.\n");
        break;
    case -2:
        wprintf(L"The file 2 is greater than the file 1.\n");
        break;
    case 2:
        wprintf(L"The file 1 is greater than the file 2.\n");
        break;
    default:
        wprintf(L"Unknown result.\n");
        break;
    }
    QueryPerformanceCounter(&endTime);
    double elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;
    wprintf(L"Execution time: %.2f millisecondes\n", elapsedTime);
    
    free(FirstFile);
    free(SecondFile);

    printf("\nPress Enter to exit.\n");
    getchar();

    return 0;
}
