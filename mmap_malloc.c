#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>

//mmmap used as dynamic memory

int main(int argc , char ** argv)
{
int N=5;    
int *ptr = mmap ( NULL,     /*   Le the OS choose the starting virtual address of the memory, just
                                                    like you do not have a control as to what address malloc() will return */
                            N*sizeof(int),      /* Size of memory in bytes being requested for allocation */ 
                             PROT_READ |  /* Memory is Readable */
                             PROT_WRITE, /* Memory is Writable */
                             MAP_PRIVATE |  /* This memory is not sharable with any other process, use MAP_SHARED instead */
                             MAP_ANONYMOUS,   /* This memory is not mapped to extern data source, but only RAM by default */
                             0,     /* FD 0, since no external data source is specified */
                             0 );  /* offset value as zero, since no extern data source is specified */
if(ptr==0)
{
    perror("mmap");
    exit(0);
}

printf("Memory allocation starts at %p\n",ptr);

int err =  munmap(ptr,10*sizeof(int));
if(err < 0)
{
    perror("munmap");
    exit(0);
}

return 0;

}