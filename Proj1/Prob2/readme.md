# Problem 2:
## Description：

Test your new system call.

- Write a simple C program which calls ptree.
- Print the entire process tree (in DFS order) using tabs to indent children with respect to their parents.
- The output format of every process is:

```c
printf(/* correct number of \t */);
printf("%s,%d,%ld,%d,%d,%d,%d\n", p.comm, p.pid, p.state,
p.parent_pid, p.first_child_pid, p.next_sibling_pid, p.uid);
```

## Requirement:

- Pure can correctly print the pstree to express the relationship among processes.

## Steps:

- The module ptree allocates enough space for required parameters buf and nr. 

- In the main() function, we use a int array **tab_array[nr]** to store the tab number, so we initiate the array as second step.
- Then, we print the number of **\t** refers to the array, and show the information of every process.

## Pictures:

![prob1](https://ws1.sinaimg.cn/large/006tNc79ly1g29iddlfyxj30zj0rtdpt.jpg)



## Files:

After building the project, we can get module name **ptree**, how to upload it can refer to [my blog](<http://kylinchen.top/2019/04/06/Ubuntu-Android_init/>).