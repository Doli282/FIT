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
    if(give_up_capabilities(necessaryCapabilities, 1) != 0)
    {
        fprintf(stderr, "%s: Unable to set capabilites.\n", argv[0]);
        exit(2);
    }

    // Check number of given arguments
    if (argc != 2)
    {
        fprintf(stderr,"%s: Invalid number of arguments: provided: %d, expected: 2\n", argv[0], argc);
        exit(1);
    }

    struct timeval newTime;
    newTime.tv_usec = 0; // Will not set precise microseconds

    char *end;
    errno = 0; // Clear error messages

    // Convert given argument
    newTime.tv_sec = strtol(argv[1], &end, 10);

    // Check if input is out of bounds
    if (errno == ERANGE)
    {
        fprintf(stderr, "%s: Invalid argument - range error.\n", argv[0]);
        exit(1);
    }
    // Check if input is a valid number
    if (*end != '\0')
    {
        fprintf(stderr, "%s: Input was not a number.\n", argv[0]);
        exit(1);
    }

    // Set time
    if(settimeofday(&newTime, NULL) != 0)
    {
        fprintf(stderr, "%s: Error during setting the time occured.\n", argv[0]);
        return 3;
    }

    return 0;
}
