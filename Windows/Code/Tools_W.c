#include <stdio.h>
#include <time.h>
#include <windows.h>

void proceed()
/*
* Includes:
*   stdio.h
* Params:
*   None
* Return:
*   None
* Description:
*   This function prints a message and wait for any button to proceed.
* Notes:
*   This clears the Terminal at the end
*/
{
    wprintf(L"\nPress any button to continue.");
    getchar();
    system("cls");
}

WCHAR* getCurrentDateWCHAR()
/*
* Includes:
*   windows.h
*   time.h
* Params:
*   None
* Return:
*   WCHAR* => The current date array(Pointor) // Ex: 2023-08-19
* Description:
*   This function returns the current date array(Pointor) /!\ the return has to be free() as it is declared with malloc /!\
* Notes:
*   /!\ the return has to be free() as it is declared with malloc /!\
*/
{
    time_t currentTime;
    struct tm timeInfo;
    WCHAR formattedDate[11];

    // Get Actual time
    time(&currentTime);

    // Convert time into a secured structure
    if (localtime_s(&timeInfo, &currentTime) != 0)
    {
        return NULL; // Error
    }

    // Date format : "YYYY-MM-DD"
    if (wcsftime(formattedDate, sizeof(formattedDate), L"%Y-%m-%d", &timeInfo) == 0)
    {
        return NULL; // Error
    }

    // Memory allocation
    WCHAR* result = (WCHAR*)malloc(sizeof(formattedDate));
    if (result == NULL)
    {
        return NULL; // Error
    }

    // uses wcscpy_s copy the formated date in result
    if (wcscpy_s(result, sizeof(formattedDate) / sizeof(formattedDate[0]), formattedDate) != 0)
    {
        free(result); // free if there's an error
        return NULL;
    }

    return result;
}

WCHAR* getCurrentTimeWCHAR()
/*
* Includes:
*   windows.h
*   time.h
* Params:
*   None
* Return:
*   WCHAR* => The current date array(Pointor) // Ex:23h12
* Description:
*   This function returns the current Time array(Pointor), the format is HHHMM(Ex: 23h12) /!\ the return has to be free() as it is declared with malloc /!\
* Notes:
*   /!\ the return has to be free() as it is declared with malloc /!\
*/
{
    time_t currentTime;
    struct tm timeInfo;
    WCHAR formattedTime[6]; // HHHMM\0

    // Get Actual time
    time(&currentTime);

    // Convert time into a secured structure
    if (localtime_s(&timeInfo, &currentTime) != 0)
    {
        return NULL; // Error
    }

    // Tinme Format : "HH-MM"
    if (wcsftime(formattedTime, sizeof(formattedTime), L"%HH%M", &timeInfo) == 0)
    {
        return NULL; // Error
    }

    // Memory allocation
    size_t resultSize = wcslen(formattedTime) + 1; // Size + 1 for the null character
    WCHAR* result = (WCHAR*)malloc(resultSize * sizeof(WCHAR));
    if (result == NULL)
    {
        return NULL; // Error
    }

    // Copier la date formatée dans le résultat
    if (wcscpy_s(result, resultSize, formattedTime) != 0)
    {
        free(result); // free if there's an error
        return NULL;
    }

    return result;
}

int choice(WCHAR* sentence1, int x, WCHAR* sentence2, int y)
/*
* Includes:
*   windows.h
* Params:
*   WCHAR* sentence1 = Description of the first choice
*   int x = Value of the first choice
*   WCHAR* sentence2 = Description of the second choice
*   int y = Value of the second choice
* Return:
*   int => The value chosen. Exclusively x or y
* Description:
*   This function returns the choice made by the user and only either x or y. The 2 sentences are here to describe the choices
*   Ex: sentence1 = L"Endou"
*       x = 1
*       sentence2 = L"Gouenji"
*       y = 11
*
*       The print will be:
*                           Endou: 1
*                           Gouenji: 11
*                           Choice:
* Notes:
*   This loops until x or y is chosen
*/
{
    int scanfResult;
    int methodChoice = 0;
    wprintf(L"\n%s: %d\n%s: %d\nChoice: ", sentence1, x, sentence2, y);
    do
    {
        scanfResult = scanf_s("%d%*c", &methodChoice);
        if (methodChoice != x && methodChoice != y)
        {
            wprintf(L"Only %d or %d.\tChoice: ", x, y);
            while (getchar() != '\n');
        }
    } while (methodChoice != x && methodChoice != y);

    wprintf(L"\n");
    return methodChoice;
}

int choiceBTW(WCHAR* sentence1, int x, WCHAR* sentence2, int y)
/*
* Includes:
*   windows.h
* Params:
*   WCHAR* sentence1 = Description of the Lowest choice
*   int x = Value of the Lowest choice
*   WCHAR* sentence2 = Description of the Greatest choice
*   int y = Value of the Greatest choice
* Return:
*   int => The value chosen. Exclusively between x or y(x and y included)
* Description:
*   This function returns the choice made by the user and only either x or y. The 2 sentences are here to describe the choices
*   Ex: sentence1 = L"To be GK do"
*       x = 1
*       sentence2 = L"To be at any position choose between 2 and 10\nTo be Ace Striker do"
*       y = 11
*
*       The print will be:
*                           To be GK do: 1
*                           To be at any position choose between 2 and 10
*                           To be Ace Striker do: 11
*                           Choice:
* Notes:
*   This loops until a value between x or y is chosen(x and y included)
*/
{
    int scanfResult;
    int methodChoice = 0;
    wprintf(L"\n%s: %d\n%s: %d\nChoice: ", sentence1, x, sentence2, y);
    do
    {
        scanfResult = scanf_s("%d%*c", &methodChoice);
        if (methodChoice < x || methodChoice > y)
        {
            wprintf(L"Only values between %d and %d are allowed.\tChoice: ", x, y);
            while (getchar() != '\n');
        }
    } while (methodChoice < x || methodChoice > y);

    wprintf(L"\n");
    return methodChoice;
}

