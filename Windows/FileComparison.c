#include <stdio.h>
#include <windows.h>

int main() {

        wprintf(L"Hello!\t Please select which mode to use:\n");
        wprintf(L"Press 1\tfor the faster one that uses Clusters of 4096 bytes.\n");
        wprintf(L"Press 2\tfor the slower but more accurate method that compares literally byte per byte.\n\n");
        wprintf(L"Choice: ");
        unsigned int choice = 0;
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

        do {

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

        printf("\nPress 0 to close or Enter (Or anything) to compare other files.\n");
    } while (getchar()!='0');

    return 0;
}
