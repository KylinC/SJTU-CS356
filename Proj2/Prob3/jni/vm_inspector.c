#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>

#define __NR_get_pagetable_layout 356
#define __NR_expose_page_table 357

#define pgd_index(va,loinfo) ((va)>>loinfo.pgdir_shift)
#define pte_index(va,loinfo) (((va)>>loinfo.page_shift)&((1<<(loinfo.pmd_shift-loinfo.page_shift))-1))

// given info struct
struct pagetable_layout_info{
    uint32_t pgdir_shift;
    uint32_t pmd_shift;
    uint32_t page_shift;
};

void print_usage(int para_num)
{
    printf("===========================================================================\n");
    printf("Parameters Wrong!\n");
    printf("Need 3 parameters, only %d detected!\n",para_num-1);
    printf("The Correct Format Can be:  ");
    printf("./vm_inspector PID BEGIN_VADRR END_VADRR\n");
    printf("===========================================================================\n");
}


int main(int argc,char **argv)
{
    pid_t pid;
    unsigned long begin_vaddr,current_va;
    unsigned long end_vaddr;
    unsigned long *table_addr,*fake_pgd_addr;
    struct pagetable_layout_info loinfo;
    unsigned long page_size;
    unsigned long rd_begin,rd_end;
    unsigned long mask;
    unsigned long page_nums;

    //check the arguments.
    if(argc!=4)
    {
        print_usage(argc);
        return -1;
    }

    pid=atoi(argv[1]);
    begin_vaddr=strtoul(argv[2],NULL,16);
    end_vaddr=strtoul(argv[3],NULL,16);
    current_va=begin_vaddr;

    //call the layout
    syscall(__NR_get_pagetable_layout, &loinfo,4*3);

    //calculate page size and calculate mask.
    page_size=1<<(loinfo.page_shift);
    mask=page_size-1;

    rd_begin=begin_vaddr&~mask;
    rd_end=(end_vaddr+mask)&~mask;

    //calculate needed pages.
    page_nums=pgd_index(rd_end-1,loinfo)-pgd_index(rd_begin,loinfo)+1;

    //allocate enough memory space.
    table_addr=mmap(NULL,page_size*page_nums,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    fake_pgd_addr=malloc(sizeof(unsigned long)*page_size);

    if(!table_addr||!fake_pgd_addr)
    {
        printf("Memory Error!\n");
        return -1;
    }

    syscall(__NR_expose_page_table,pid,fake_pgd_addr,0,table_addr,begin_vaddr,end_vaddr);

    // in loop variable
    unsigned long table_number;
    unsigned long page_frame_number;
    unsigned long pgd_ind,phy_addr,vir_addr;
    unsigned long *phy_base;
    unsigned long begin_table_number=rd_begin>>loinfo.page_shift, end_table_number=rd_end>>loinfo.page_shift;

    //print location va,pa,pn,fn
    printf("===============================Translation=================================\n");
    printf("Virtual Address\t\tPhysical Address\tPage Number\tFrame Number\n");
    for(table_number=begin_table_number;table_number<end_table_number;++table_number)
    {
    	vir_addr=table_number<<loinfo.page_shift;
        pgd_ind=pgd_index(vir_addr,loinfo);
        phy_base=fake_pgd_addr[pgd_ind];
        if(phy_base)
        {
            phy_addr=phy_base[pte_index(vir_addr,loinfo)];
            page_frame_number=phy_addr>>loinfo.page_shift;
            if(page_frame_number)
            {
                printf("0x%08lx\t\t0x%08lx\t\t0x%08lx\t0x%08lx\n",vir_addr,phy_addr,table_number,page_frame_number);
            }
        }
    }
    printf("===========================================================================\n");

    //free memory space.
    free(fake_pgd_addr);
    munmap(table_addr,page_size*page_nums);
    return 0;
}
