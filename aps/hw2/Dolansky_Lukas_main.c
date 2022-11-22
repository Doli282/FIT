#include <unistd.h>
#include <time.h>

int main(int argc, char **argv)
{
    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);








    clock_gettime(CLOCK_REALTIME, &stop);
    double accum = (stop.tv_sec - start.tv_sec) * 1000.0 + (stop.tv_nsec - start.tv_nsec) / 1000000.0;
    printf("Time: %.6lf ms\n", accum);

    return 0;
}