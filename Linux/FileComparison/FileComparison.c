#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include "FilesFuncsLU.h"
#include "ToolsW.h"

int main() {

    printf("Hello!\t Please select which mode to use:\n");
    printf("Press 1\tfor the faster one that uses Clusters of 4096 bytes.\n");
    printf("Press 2\tfor the slower but more accurate method that compares literally byte per byte.\n\n");
    printf("Choice: ");
    unsigned int choice=0;
    int scanfResult;
    do
    {
        scanfResult = scanf("%d%*c", &choice);
        if (scanfResult != 1 || (choice != 1 && choice != 2))
        {
            printf("Only 1 or 2.\tChoice: ");
            while(getchar() != '\n');
        }
    } while (choice != 1 && choice != 2);

    do {
        printf("Please Select the 1st file to compare.\n");

        char *FirstFile = selectFilePath();
        if (FirstFile == NULL) {
            g_free(FirstFile);
            return -1;
        }
        printf("The selected file is: %s\n\n", FirstFile);

        printf("Please Select the 2nd file to compare.\n");

        char *SecondFile = selectFilePath();
        if (SecondFile == NULL) {
            free(SecondFile);
            return -1;
        }
        printf("The selected file is: %s\n\n", SecondFile);


        long long int startTime, endTime, elapsedTime;
        long long int frequency = 1000000000LL; // Fréquence en nanosecondes par seconde

        startTime = getCurrentTimeInNanos();

        switch ((choice == 2) ? compareFiles(FirstFile, SecondFile) : compareFilesCluster(FirstFile, SecondFile)) {
            case -1:
                printf("The files are different.\n");
                break;
            case 1:
                printf("The files are identical.\n");
                break;
            case -2:
                printf("The file 2 is greater than the file 1.\n");
                break;
            case 2:
                printf("The file 1 is greater than the file 2.\n");
                break;
            default:
                printf("Unknown result.\n");
                break;
        }

        endTime = getCurrentTimeInNanos();
        elapsedTime = endTime - startTime;
        double elapsedTimeMilliseconds = (double) elapsedTime / 1000000.0;
        printf("Execution time: %.2f millisecondes\n", elapsedTimeMilliseconds);
        free(FirstFile);
        free(SecondFile);

        printf("\nPress 0 to close or Enter (Or anything) to compare other files.\n");
    }while (getchar()!='0');

    return 0;
}