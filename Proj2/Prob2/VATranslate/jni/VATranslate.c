#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/mman.h>

#define __NR_get_pagetable_layout 356
#define __NR_expose_page_table 357

// given info struct
struct pagetable_layout_info
{
    uint32_t pgdir_shift;
    uint32_t pmd_shift;
    uint32_t page_shift;
};

void print_usage(int para_num)
{
    printf("============================================================\n");
    printf("Parameters Wrong!\n");
    printf("Need 2 parameters, only %d detected!\n",para_num-1);
    printf("The Correct Format Can be:  ");
    printf("./begin_vaddrTranslate #PID #VA\n");
    printf("============================================================\n");
}

void display_pagetable_layout(struct pagetable_layout_info* layout)
{
    printf("============================================================\n");
    printf("Android Pagetable Layout:\n");
    printf("  pgdir_shift = %d\n", layout->pgdir_shift);
    printf("  pmd_shift = %d\n", layout->pmd_shift);
    printf("  page_shift = %d\n", layout->page_shift);
}

void display_pagetable_expose(pid_t pid, unsigned long *fake_pgd_addr, unsigned long *page_table_addr, unsigned long begin_vaddr, unsigned long end_vaddr, unsigned page_size)
{
    unsigned long pgd_ind,pte_ind,phy_addr,mask=page_size-1;
    unsigned long *phy_base;

    // call the expose fuction
    syscall(__NR_expose_page_table, pid, fake_pgd_addr, 0, page_table_addr, begin_vaddr, end_vaddr);

    //get pgd index.
    pgd_ind = (begin_vaddr >> 21) & 0x7FF;
    pte_ind = (begin_vaddr >> PAGE_SHIFT) & 0x1FF;

    //get physical base of page table.
    phy_base=fake_pgd_addr[pgd_ind];

    if(phy_base)
    {
        //get the entry in the table.
        phy_addr=phy_base[pte_ind];

        //mask the lower bit of the entry.
        phy_addr=phy_addr&~mask;
        if(phy_addr)
        {
            phy_addr=begin_vaddr&mask|phy_addr;
            printf("Virtual Address Translation:\n");
            printf("  virtual address = 0x%08lx\n", begin_vaddr);
            printf("  physical address = 0x%08lx\n", phy_addr);
            printf("============================================================\n");
        }
        else 
        {
            printf("virtual address:0x%08lx not locate in the memory.\n",begin_vaddr);
            printf("============================================================\n");
        }
    }
    else 
    {
        printf("virtual address:0x%08lx not locate in the memory.\n",begin_vaddr);
        printf("============================================================\n");
    }

}

int main(int argc,char **argv)
{
    // catch the para-fault_infoor exception
    if(argc!=3)
    {
        print_usage(argc);
        return -1;
    }

    pid_t pid;  // parameter PID
    unsigned long begin_vaddr, end_vaddr;  // parameter begin_vaddr
    struct pagetable_layout_info pagetable_info; // info struct

    unsigned long *page_table_addr;
    unsigned long *fake_pgd_addr;
    unsigned page_size;

    pid=atoi(argv[1]);
    begin_vaddr=strtoul(argv[2],NULL,16);
    end_vaddr=begin_vaddr+1;

    // get the info struct call get layout.
    syscall(__NR_get_pagetable_layout,&pagetable_info,4*3);

    //print the layout offset.
    display_pagetable_layout(&pagetable_info);

    //get address
    page_size=1<<(pagetable_info.page_shift);
    page_table_addr=mmap(NULL,page_size,PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    fake_pgd_addr=malloc(sizeof(unsigned long)*page_size);

    /////////////////////////
    display_pagetable_expose(pid, fake_pgd_addr, page_table_addr, begin_vaddr, end_vaddr, page_size);
    
    //free memory space.
    free(fake_pgd_addr);
    munmap(page_table_addr,page_size);
    return 0;
}
