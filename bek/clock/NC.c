#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

// Include file with prepared function to change Linux capabilites
#include "capabilities.c"

int main(int argc, char *argv[])
{
    // Set only necessary Linux capabilites - needed capabilities are:
    // CAP_SYS_TIME  .... for settimeofday()
    cap_value_t necessaryCapabilities[1] = {CAP_SYS_TIME};
    give_up_capabilities(necessaryCapabilities, 1);

    // Check number of given arguments
    if (argc != 2)
    {
        exit(1);
    }

    struct timeval newTime;
    newTime.tv_usec = 0; // Will not set precise microseconds

    char *end;
    errno = 0; // Clear error messages

    // Convert given argument
    newTime.tv_sec = strtol(argv[1], &end, 10);

    // Check if input was correctly converted to a number
    if (errno != 0)
    {
        exit(1);
    }
    // Check if input is a valid number
    if (*end != '\0')
    {
        exit(1);
    }

    // Set time
    if(settimeofday(&newTime, NULL) != 0)
    {
        return 3;
    }

    return 0;
}
