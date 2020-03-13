#ifndef _V_MEM_ALLOC_
#define _V_MEM_ALLOC_
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define VM_MEM_SIZE 3000
#define VM_ALIGNMENT_SIZE 32



#if      (VM_ALIGNMENT_SIZE == 32)
    #define VM_ALIGNMENT_MASK   (0x001F)
#elif    (VM_ALIGNMENT_SIZE == 16)
    #define VM_ALIGNMENT_MASK   (0x000F)
#elif    (VM_ALIGNMENT_SIZE == 8)
    #define VM_ALIGNMENT_MASK   (0x0007)
#elif    (VM_ALIGNMENT_SIZE == 4)
    #define VM_ALIGNMENT_MASK   (0x0003)
#elif    (VM_ALIGNMENT_SIZE == 2)
    #define VM_ALIGNMENT_MASK   (0x0001)
#elif    (VM_ALIGNMENT_SIZE == 1
    #define VM_ALIGNMENT_MASK   (0x0000)
#endif


void* vMemAlloc( size_t allocSize );
void vMemFree( void* pv );
size_t vMemSizeOf(void * pr);
void vMemInfoPrint(void);


#ifdef __cplusplus
}
#endif
#endif // !_V_MEM_ALLOC_
