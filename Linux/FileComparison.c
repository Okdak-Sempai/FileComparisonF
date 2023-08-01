//Compile
//gcc `pkg-config --cflags gtk+-3.0` -o FileComparison FileComparison.c `pkg-config --libs gtk+-3.0` && ./FileComparison ; echo "Return code: $?"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sys/stat.h>
#include <time.h>

off_t getFileSize(const char* filePath) {
    struct stat st;
    if (stat(filePath, &st) == 0) {
        return st.st_size;
    } else {
        return -1;
    }
}

char* selectFilePath() {
    gtk_init(NULL, NULL);

    GtkWidget* dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Open File", NULL, action, "_Cancel",
                                        GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char* filename;
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        gtk_widget_destroy(dialog);
        return filename;
    }

    gtk_widget_destroy(dialog);
    return NULL;
}

int compareFiles(const char* file1, const char* file2) {
    off_t file1Size = getFileSize(file1);
    off_t file2Size = getFileSize(file2);

    off_t mainfile = (file1Size > file2Size) ? file1Size : file2Size;
    printf("File 1 size: %lu\n", file1Size);
    printf("File 2 size: %lu\n", file2Size);
    if (file1Size != file2Size)
    {
        printf("Files size aren't even alike.");
        return -1;
    }
    FILE* fptr1 = fopen(file1, "rb");
    if (fptr1 == NULL) {
        perror("Error opening file 1");
        return -1;
    }

    FILE* fptr2 = fopen(file2, "rb");
    if (fptr2 == NULL) {
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
            printf("\n");
            return 1; // Files are identical.
        }

        if (byte1 == EOF || byte2 == EOF) {
            fclose(fptr1);
            fclose(fptr2);
            if (byte1 == EOF) {
                printf("\n");
                return -2; // OEF of file 1 before 2.
            }
            else {
                printf("\n");
                return 2; // OEF of file 2 before 1.
            }
        }

        if (byte1 != byte2) {
            fclose(fptr1);
            fclose(fptr2);
            printf("\n");
            return -1; // Files are different.
        }

        position++;
        printf("\rProgression: %d%%", (int)(((float)position/mainfile) * 100));
    }
}

int compareFilesCluster(const char* file1, const char* file2) {
    off_t file1Size = getFileSize(file1);
    off_t file2Size = getFileSize(file2);

    off_t mainfile = (file1Size > file2Size) ? file1Size : file2Size;
    printf("File 1 size: %lu\n", file1Size);
    printf("File 2 size: %lu\n", file2Size);
    if (file1Size != file2Size)
    {
        printf("Files size aren't event alike.\n");
        return -1;
    }

    FILE* fptr1 = fopen(file1, "rb");
    if (fptr1 == NULL) {
        perror("Error opening file 1");
        return -1;
    }

    FILE* fptr2 = fopen(file2, "rb");
    if (fptr2 == NULL) {
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
            printf("\n");
            return -1; // Files are different.
        }

        position += bytesRead1;
        printf("\rProgression: %d%%", (int)(((float)position / mainfile) * 100));
        if (bytesRead1 < 4096) {
            printf("\n");
            break;
        }
    }

    fclose(fptr1);
    fclose(fptr2);

    return 1;  // Files are identical.
}

long long int getCurrentTimeInNanos() {
    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    return (long long int)currentTime.tv_sec * 1000000000LL + currentTime.tv_nsec;
}

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

    printf("Please Select the 1st file to compare.\n");

    char* FirstFile = selectFilePath();
    if (FirstFile == NULL)
    {
        g_free(FirstFile);
        return -1;
    }
    printf("The selected file is: %s\n\n", FirstFile);

    printf("Please Select the 2nd file to compare.\n");

    char* SecondFile = selectFilePath();
    if (SecondFile == NULL)
    {
        free(SecondFile);
        return -1;
    }
    printf("The selected file is: %s\n\n", SecondFile);


    long long int startTime, endTime, elapsedTime;
    long long int frequency = 1000000000LL; // Fréquence en nanosecondes par seconde

    startTime = getCurrentTimeInNanos();

    switch((choice==2)? compareFiles(FirstFile, SecondFile) : compareFilesCluster(FirstFile, SecondFile))
    {
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
    double elapsedTimeMilliseconds = (double)elapsedTime / 1000000.0;
    printf("Execution time: %.2f millisecondes\n", elapsedTimeMilliseconds);
    free(FirstFile);
    free(SecondFile);

    printf("\nPress Enter to exit.\n");
    getchar();

    return 0;
}
