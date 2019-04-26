#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/unistd.h>
/*list*/
#include <linux/list.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

MODULE_LICENSE("Dual BSD/GPL");
#define __NR_ptree 356
#define BUFFER 2048

struct prinfo {
	pid_t parent_pid; /* process id of parent */
	pid_t pid; /* process id */
	pid_t first_child_pid; /* pid of youngest child */
	pid_t next_sibling_pid; /* pid of older sibling */
	long state; /* current state of process */
	long uid; /* user id of process owner */
	char comm[16]; /* name of program executed */
};


void dfs(struct task_struct* old, struct prinfo* new, int* p){
	/*recursive call variable*/
	struct list_head* head;
	struct task_struct* task;
    
    /*construct init*/
    /*copy parent*/
	new[*p].parent_pid = (old->parent)?old->parent->pid:0;

    /*copy self*/
	new[*p].pid=old->pid;

	/*copy child and its sibling*/
	new[*p].first_child_pid=(!list_empty(&(old->children)))?list_entry(old->children.next, struct task_struct, sibling)->pid:0;
	new[*p].next_sibling_pid=(!list_empty(&(old->sibling)))?list_entry(old->sibling.next, struct task_struct, sibling)->pid:0;

	/*copy other state*/
	new[*p].state = old->state;
	new[*p].uid = old->cred->uid;

	get_task_comm(new[*p].comm, old);
    
    /*construct end*/

    ++(*p);
	list_for_each(head, &(old->children)){
		task=list_entry(head,struct task_struct, sibling);
		//recurssive call dfs() itself
		dfs(task, new, p);
	}
}


/* problem required system call */
static int (*oldcall)(void);
int ptree(struct prinfo *buf,int *nr){
    struct prinfo *buffer = (struct prinfo*)kmalloc(sizeof(struct prinfo)* BUFFER, GFP_KERNEL);
    /* recurssive flag value */
    int p=0;
    /* copy the data from kernel */
    read_lock(&tasklist_lock);/* unlock */
	dfs(&init_task,buffer,&p);
	read_unlock(&tasklist_lock);/* lock */

    /* copy the data from kernel */
    if(copy_to_user(buf, buffer, sizeof(struct prinfo)* BUFFER) || copy_to_user(nr, &p, sizeof(int)))
	{
		printk(KERN_INFO "ERROR!\n");
		return 1;
	}

	//release
	kfree(buffer);
	return 0;
}


static int addsyscall_init(void)
{
	long *syscall = (long*)0xc000d8c4;
	oldcall = (int(*)(void))(syscall[__NR_ptree]);
	syscall[__NR_ptree] = (unsigned long)ptree;
	printk(KERN_INFO "module load!\n");
	return 0;
}
static void addsyscall_exit(void)
{
	long *syscall = (long*)0xc000d8c4;
	syscall[__NR_ptree] = (unsigned long)oldcall;
	printk(KERN_INFO "module exit!\n");

}

module_init(addsyscall_init);
module_exit(addsyscall_exit);
