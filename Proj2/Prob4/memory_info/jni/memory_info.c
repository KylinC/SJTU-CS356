#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* 
 * This program run as a memory monitor, which print information from /proc/meminfo
 * And the last two line contains active and inactive memory we need
 */

void examine()
{
    int loopnum;
    char s[256];
    freopen("/proc/meminfo", "r", stdin);
    for(loopnum=0;loopnum<7;++loopnum)
    {
        fgets(s, 256, stdin);
        printf("%s", s);
    }
}

int main(int argc, char *argv[])
{
    int i;
    for (i = 0; i <= 30; ++i)
    {
        printf("========================================================\n");
        printf("%d-th second check: \n", i);
        examine();
        sleep(1);
    }
    return 0;
}
