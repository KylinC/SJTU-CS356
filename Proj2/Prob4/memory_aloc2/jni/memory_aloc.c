#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/mman.h>

/*
 * This memory_aloc.c try to alloc 512M memory to awake kawapd process
 * You should use memory_info to get the memory infomation while running
 */

int main()
{
    unsigned long **p;
    unsigned long i, j;
	printf("Try to alloc 512M Memory.\n");
    p = (unsigned long**)malloc(1 << 12);
    if (!p)
    	printf("Low Memory Error.\n");
    else
    {
        for (i = 0; i < (1 << 12); ++i)
        {
        	p[i] = (unsigned long*)malloc(1 << 17);
        }
        for (j = 0; j < 32; ++j)
        {
        	for (i = 0; i < (1 << 12); ++i)
        		p[i][j << 10] = 0;
        }
        printf("Releasing after 2s.\n");
        sleep(2);
        for (i = 0; i < (1 << 12); ++i)
        	if(p[i])
        		free(p[i]);
    }
    if (p)
    	free(p);
	return 0;
}
