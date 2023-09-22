#include <time.h>

long long int getCurrentTimeInNanos()
/*
 * Includes:
 *  time.h
 * Params:
 *  None
 * Return:
 * long long int => Current time ine nano seconds
 * Description:
 *  This function returns the current time in nano seconds
 * Notes:
 *  None
 */
{
    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    return (long long int)currentTime.tv_sec * 1000000000LL + currentTime.tv_nsec;
}