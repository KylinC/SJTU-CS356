#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	pid_t pid = getpid();
	pid_t child_pid = fork();

	if(!child_pid)
	{
		/*This is child pid*/
		printf("===============Below is child pid=================\n");
        printf("ID: 517030910155 \t PID:%d\n", getpid());
		printf("Execute ptree in this content:\n");
		execl("./ptree", "ptree", NULL);
		_exit(0);
	}
	else
	{
		/*This is parent pid*/
		printf("==============Below is parent pid=================\n");
		printf("ID: 517030910155 \t PID:%d\n", pid);
		printf("==================================================\n");
		/*wait*/
		wait(NULL);
	}
	return 0;
}