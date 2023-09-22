#include <gtk/gtk.h>
#include <sys/stat.h>
#include "FilesFuncsLU.h"

char* selectFilePath()
/*
* Includes: gtk/gtk.h
* params:
*   None
* Return:
*   char* => The path of the file chosen by the user
* Description:
*   This function open the file explorer for windows and let the user choice a file, the path of the file is returned
* Notes:
*   None
*/
{
    gtk_init(NULL, NULL);

    GtkWidget* dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Open File", NULL, action, "_Cancel",
                                         GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char* filename;
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        gtk_widget_destroy(dialog);
        return filename;
    }

    gtk_widget_destroy(dialog);
    return NULL;
}

off_t getFileSize(const char* filePath)
/*
* Includes:
*   sys/stat.h
* params:
*   const char* filePath = The path of a file
* Return:
*   off_t => Size of the filePath file
* Description:
*   This function returns the size of a file or -1 if there's an error
* Notes:
*   None
 */
{
    struct stat st;
    if (stat(filePath, &st) == 0)
    {
        return st.st_size;
    } else
    {
        return -1;
    }
}

int compareFiles(const char* file1, const char* file2)
/*
* Includes:
*   sys/stat.h
* params:
*   const char* file1 = The path of the 1st file to compare
*   const char* file2 = The path of the 2nd file to compare
* Return:
*   int
*       -1 => if the files are different or if there's an error opening the file
*       -2 => if there's an OEF of file 1 before file 2
*       2 => if there's an OEF of file 2 before file 1
*       1 => if the files are identical
* Description:
*   This function compares file1 and file2 byte per byte and return the result as an int.
* Notes:
*   It includes a progression bar
*/
{
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
    if (fptr1 == NULL)
    {
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
    while (1)
    {
        byte1 = fgetc(fptr1);
        byte2 = fgetc(fptr2);

        if (byte1 == EOF && byte2 == EOF)
        {
            fclose(fptr1);
            fclose(fptr2);
            printf("\n");
            return 1; // Files are identical.
        }

        if (byte1 == EOF || byte2 == EOF)
        {
            fclose(fptr1);
            fclose(fptr2);
            if (byte1 == EOF) {
                printf("\n");
                return -2; // OEF of file 1 before 2.
            }
            else
            {
                printf("\n");
                return 2; // OEF of file 2 before 1.
            }
        }

        if (byte1 != byte2)
        {
            fclose(fptr1);
            fclose(fptr2);
            printf("\n");
            return -1; // Files are different.
        }

        position++;
        printf("\rProgression: %d%%", (int)(((float)position/mainfile) * 100));
    }
}

int compareFilesCluster(const char* file1, const char* file2)
/*
* Includes:
*   sys/stat.h
* params:
*   const char* file1 = The path of the 1st file to compare
*   const char* file2 = The path of the 2nd file to compare
* Return:
*   int
*       -1 => if the files are differents or if there's an error opening the file
*       1 => if the files are identical
* Description:
*   This function compares file1 and file2 by batch of 4096 byte and return the result as an int.
* Notes:
*   It includes a progression bar
*/
{
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
    if (fptr1 == NULL)
    {
        perror("Error opening file 1");
        return -1;
    }

    FILE* fptr2 = fopen(file2, "rb");
    if (fptr2 == NULL)
    {
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

        if (bytesRead1 != bytesRead2 || memcmp(buffer1, buffer2, bytesRead1) != 0)
        {
            fclose(fptr1);
            fclose(fptr2);
            printf("\n");
            return -1; // Files are different.
        }

        position += bytesRead1;
        printf("\rProgression: %d%%", (int)(((float)position / mainfile) * 100));
        if (bytesRead1 < 4096)
        {
            printf("\n");
            break;
        }
    }

    fclose(fptr1);
    fclose(fptr2);

    return 1;  // Files are identical.
}