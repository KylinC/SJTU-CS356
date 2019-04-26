#include <string.h>
#include <stdlib.h>
#include "stdio.h"

#define BUFFER 2048
#define PIDSIZE 256

struct prinfo
{
	pid_t parent_pid;
	pid_t pid;
	pid_t first_child_pid;
	pid_t next_sibling_pid;
	long state;
	long uid;
	char comm[16];
};

int main(int argc, char *argv[]){
    /*flag1*/
    //printf("Entering main function.\n\n");

	struct prinfo *buf = (struct prinfo*)malloc(sizeof(struct prinfo)* BUFFER);
	int nr;
	syscall(356, buf, &nr);// syscall ptree
	
    /*print ptree*/

    /*store the number of child process*/
    int tmp_array[PIDSIZE];
    /*store the number of /t token*/
    int tab_array[PIDSIZE];

    /*array init*/
    memset(tmp_array,0,PIDSIZE*sizeof(int));
    memset(tab_array,0,PIDSIZE*sizeof(int));

    int tab_tmp=0;

    /*erdefinition will be blocked*/
    int i,j;

    for(i=0;i<nr;++i){
        //printf("loop number: %d\n",i);
    	if(buf[i].next_sibling_pid != buf[i+1].pid && buf[i].first_child_pid != buf[i+1].pid){
    		buf[i].next_sibling_pid = 0;
    	}
    	if(i == nr-1){
    		buf[i].next_sibling_pid = 0;
    	}
    	if(tab_tmp==0 || buf[i].pid == buf[tmp_array[tab_tmp-1]].first_child_pid){
    		tmp_array[tab_tmp++] = i;
    	}
    	else{
    		while(buf[i].pid != buf[tmp_array[tab_tmp-1]].next_sibling_pid){
    			tab_tmp--;
    			buf[tmp_array[tab_tmp]].next_sibling_pid=0;
    		}
            tmp_array[tab_tmp-1]=i;
    	}
        tab_array[i]=tab_tmp-1;
    }
    /*flag3*/
    printf("tab read finished. Total Process Number: %d\n\n",nr);

    /*format print*/
    for(i=0;i<nr;++i){
    	for(j=0;j<tab_array[i];++j){
    		printf("\t");
    	}
    	printf("%s,%d,%ld,%d,%d,%d,%ld\n", buf[i].comm, buf[i].pid, buf[i].state, buf[i].parent_pid, buf[i].first_child_pid, buf[i].next_sibling_pid, buf[i].uid);
    }

	free(buf);  //release buf
	return 0;
}
