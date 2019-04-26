# Problem 1:
## Description：

Write a new system call in Android.

- The system call you write should take two arguments
and return the process tree information in a depthfirst-search (DFS) order.

- Each system call must be assigned a number. Your
system call should be assigned number 391 ( changes to 356 according to TAs ).

## Requirement:

- The prototype for your system call will be:
  - int ptree(struct prinfo *buf, int *nr);
- You should define struct prinfo as:

```c
struct prinfo {
pid_t parent_pid; /* process id of parent */
pid_t pid; /* process id */
pid_t first_child_pid; /* pid of youngest child */
pid_t next_sibling_pid; /* pid of older sibling */
long state; /* current state of process */
long uid; /* user id of process owner */
char comm[64]; /* name of program executed */
};
```
## Tips:

- The argument buf points to a buffer for the process data,
and nr points to the size of this buffer (number of
entries). The system call copies as many entries of the
process tree data to the buffer as possible, and stores
the number of entries actually copied in nr. 

- Linux maintains a list of all processes in a doubly linked list. Each
entry in this list is a task_struct structure, which is defined in
include/linux/sched.h. When traversing the process tree data
structures, it is necessary to prevent the data structures from
changing in order to ensure consistency. 

- For this purpose the kernel relies on a special lock, the tasklist_lock.
You should grab this lock before you begin the traversal, and only
release the lock when the traversal is completed. While holding the
lock, your code may not perform any operations that may result in a
sleep, such as memory allocation, copying of data into and out from
the kernel etc. Use the following code to grab and then release the
lock:

```c
read_lock(&tasklist_lock);
...
...
read_unlock(&tasklist_lock);
```

## Pictures:

![prob1](https://ws1.sinaimg.cn/large/006tNc79ly1g29iddstg8j30kw0e7411.jpg)

## Files:

After building the project, we can get module name **sys_MyPtree.ko**, how to upload it can refer to [my blog](<http://kylinchen.top/2019/04/06/Ubuntu-Android_init/>).

