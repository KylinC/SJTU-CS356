#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h>
#include<linux/unistd.h>
/*list*/
#include <linux/list.h>
#include<linux/slab.h>
#include<asm/uaccess.h>
/*added modified extern walk_page_range*/
#include<linux/mm.h>
MODULE_LICENSE("Dual BSD/GPL");

#define __NR_syscall1 356
#define __NR_syscall2 357

// debug returning information

/*given index struct*/
struct pagetable_layout_info{
    uint32_t pgdir_shift;
    uint32_t pmd_shift;
    uint32_t page_shift;
};

/*prototype of storing coping information*/
struct walk_info
{
    unsigned long page_table_addr;
    unsigned long fake_pgd;
    unsigned long *copied_pgd;
};

static int (*oldcall1)(void);
static int (*oldcall2)(void);
static int get_pagetable_layout(struct pagetable_layout_info __user* pgtbl_info,int size)
{
    struct pagetable_layout_info layout;
    printk(KERN_INFO "Syscall get_pagetable_layout() invoked!\n");

    // exception handling
    if (size < sizeof(struct pagetable_layout_info))
        return -EINVAL;

    // get the pagetable layout information
    layout.pgdir_shift = PGDIR_SHIFT;
    layout.pmd_shift = PMD_SHIFT;
    layout.page_shift = PAGE_SHIFT;

    // copy the acquired information to user space
    if (copy_to_user(pgtbl_info, &layout, sizeof(struct pagetable_layout_info)))
        return -EFAULT;
    return 0;
}

int callback_pgd(pmd_t *pgd,unsigned long addr,unsigned long end,struct mm_walk *walk)
{
    unsigned long pgdInd=pgd_index(addr);
    unsigned long pgdpg=pmd_page(*pgd);

    //get the physical frame number.
    unsigned long pfn=page_to_pfn((struct page *)pgdpg);

    if(pgd_none(*pgd)||pgd_bad(*pgd)||!pfn_valid(pfn))
    {
        return -EINVAL;
    }

    struct walk_info *record_data=walk->private;
    //printk(KERN_INFO"pfn:%08X\n",pfn);

    struct vm_area_struct *user_vma=current->mm->mmap;
    if(!user_vma)
    {
        return -EINVAL;
    }

    //lock semaphore.
    down_write(&current->mm->mmap_sem);
    //remap the whole memory frame to userspace.
    remap_pfn_range(user_vma,record_data->page_table_addr,pfn,PAGE_SIZE,user_vma->vm_page_prot);
    //unlock semaphore.
    up_write(&current->mm->mmap_sem);

    //construct temp fake pgd in kernel space.
    record_data->copied_pgd[pgdInd]=record_data->page_table_addr;
    record_data->page_table_addr+=PAGE_SIZE;
    return 0;
}

int expose_page_table(pid_t pid,unsigned long fake_pgd,unsigned long fake_pmds,unsigned long page_table_addr,unsigned long begin_vaddr,unsigned long end_vaddr)
{
    // init the struct(s) we used
    struct pid* pid_struct;
    struct task_struct *target_process;
    struct vm_area_struct *vm_tmp;
    struct mm_walk walk={};
    struct walk_info record_data={};

    // check the memory address
    if(begin_vaddr>=end_vaddr)
    {
        printk(KERN_INFO"Invalid Mapped Address!\n");
        return -1;
    }

    // get pid from expose_page_table parameter
    pid_struct=find_get_pid(pid);
    if(!pid_struct)
    {
        return -1;
    }

    // get pid task by get_pid_task() method and printk the name.
    target_process=get_pid_task(pid_struct,PIDTYPE_PID);
    printk(KERN_INFO "Target Process -> %s \n", target_process->comm);

    // init record info and alloc memory then check
    record_data.page_table_addr=page_table_addr;
    record_data.fake_pgd=fake_pgd;
    record_data.copied_pgd=kcalloc(PAGE_SIZE,sizeof(unsigned long),GFP_KERNEL);
    if(!record_data.copied_pgd)
    {
        printk(KERN_INFO"Memory Error!\n");
        return -1;
    }
    
    // init walk struct
    walk.mm=target_process->mm;
    walk.pgd_entry=&callback_pgd;
    walk.private=&record_data;

    // print target's virtual memory address
    printk(KERN_INFO"Virtual Memory:\n");
    //unlock semaphore.
    down_write(&target_process->mm->mmap_sem);
    for(vm_tmp=target_process->mm->mmap;vm_tmp;vm_tmp=vm_tmp->vm_next)
    {
        printk(KERN_INFO"%08X->%08X\n",vm_tmp->vm_start,vm_tmp->vm_end);
    }
    //unlock semaphore.
    up_write(&target_process->mm->mmap_sem);

    current->mm->mmap->vm_flags|=VM_SPECIAL;

    //unlock semaphore.
    down_write(&target_process->mm->mmap_sem);
    // walk the page table
    walk_page_range(begin_vaddr,end_vaddr,&walk);
    //lock semaphore.
    up_write(&target_process->mm->mmap_sem);
    
    //copy the temp fake pgd to userspace.
    if(copy_to_user(fake_pgd,record_data.copied_pgd,sizeof(unsigned long)*PAGE_SIZE))
    { 
        return -1;
    }

    kfree(record_data.copied_pgd);
    return 0;
}

static int addsyscall_init(void)
{
    long *syscall=(long*)0xc000d8c4;
    oldcall1=(int(*)(void))(syscall[__NR_syscall1]);
    oldcall2=(int(*)(void))(syscall[__NR_syscall2]);
    syscall[__NR_syscall1]=(unsigned long)get_pagetable_layout;
    syscall[__NR_syscall2]=(unsigned long)expose_page_table;
    printk(KERN_INFO "module load!\n");
    return 0;
}

static void addsyscall_exit(void)
{
    long *syscall=(long*)0xc000d8c4;
    syscall[__NR_syscall1]=(unsigned long)oldcall1;
    syscall[__NR_syscall2]=(unsigned long)oldcall2;
    printk(KERN_INFO "module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);