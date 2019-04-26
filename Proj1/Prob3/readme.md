# Problem 3:
## Description：

- Generate a new process and output “StudentIDParent” with PID, then generates its children process output “StudentIDChild” with PID.
- Use execl to execute ptree in the child process，show the relationship between above two process.

## Requirement:

- Correctly call the parent and child process.

## Tips:

- The function **excel** can be used ad follows:

```c
#include<unistd.h>
main()
{
execl(“/bin/ls”,”ls”,”-al”,”/etc/passwd”,(char * )0);
}
```

## Pictures:

![prob1](https://ws1.sinaimg.cn/large/006tNc79ly1g29iddgpzbj30zg0qeajs.jpg)

## Files:

After building the project, we can get module name **process_generator**, how to upload it can refer to [my blog](<http://kylinchen.top/2019/04/06/Ubuntu-Android_init/>).

