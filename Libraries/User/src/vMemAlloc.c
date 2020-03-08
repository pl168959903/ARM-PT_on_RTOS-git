/*
██████╗ ██╗   ██╗    ███████╗ ██████╗ ██████╗
██╔══██╗╚██╗ ██╔╝    ╚══███╔╝██╔════╝ ██╔══██╗
██████╔╝ ╚████╔╝       ███╔╝ ██║  ███╗██████╔╝
██╔══██╗  ╚██╔╝       ███╔╝  ██║   ██║██╔══██╗
██████╔╝   ██║       ███████╗╚██████╔╝██║  ██║
╚═════╝    ╚═╝       ╚══════╝ ╚═════╝ ╚═╝  ╚═╝

*/
#include "vMemAlloc.h"

//預分配空間
static uint8_t memArr[ VM_MEM_SIZE ];

//鏈表結構
typedef struct stBlock {
    struct stBlock* nextBlock;
    size_t          size;
} stBlock_T;

//鏈表大小;對齊記憶體
static const size_t xBlockStructSize = ( sizeof( stBlock_T ) + ( size_t )( VM_ALIGNMENT_SIZE - 1 ) ) & ~( ( size_t )VM_ALIGNMENT_MASK );

//鏈表起點
stBlock_T pxStart, *pxEnd = NULL;

/**
 * @brief  初始化
 * @note
 * @retval None
 */
static void vMemInit( void ) {
    stBlock_T* newBlock;
    uint8_t*   pAlignedAddr;
    size_t     xTotalSize = VM_MEM_SIZE;
    size_t     uAddr      = ( size_t )memArr;

    //對齊記憶體
    if ( ( uAddr & VM_ALIGNMENT_MASK ) != 0 ) {
        uAddr += ( size_t )( VM_ALIGNMENT_SIZE - 1 );
        uAddr &= ~( ( size_t )( VM_ALIGNMENT_MASK ) );
        xTotalSize -= ( uAddr - ( size_t )memArr );
    }

    // 設定pxStart
    pAlignedAddr      = ( uint8_t* )uAddr;
    pxStart.nextBlock = ( void* )pAlignedAddr;
    pxStart.size      = ( size_t )0;

    //設定pxEnd
    xTotalSize -= xBlockStructSize;
    uAddr            = ( size_t )pAlignedAddr + xTotalSize;
    pxEnd            = ( void* )uAddr;
    pxEnd->nextBlock = NULL;
    pxEnd->size      = 0;

    //設定第一個block
    newBlock            = ( void* )pxStart.nextBlock;
    newBlock->nextBlock = pxEnd;
    newBlock->size      = ( size_t )( newBlock->nextBlock ) - ( ( size_t )newBlock + xBlockStructSize );
}
/**
 * @brief  分配空間
 * @note   
 * @param  allocSize: 分配大小
 * @retval 分配空間指標
 */
void* vMemAlloc( size_t allocSize ) {
    stBlock_T* pVisitBlock         = NULL;
    stBlock_T* pPreviousVisitBlock = NULL;
    stBlock_T* pAllocBlock         = NULL;
    stBlock_T* pPreviousAllocBlock = NULL;
    stBlock_T* pNewBlock           = NULL;
    void*      pReturn             = NULL;
    size_t     xTotleAllocSize     = allocSize + xBlockStructSize;

    //檢查是否需要初始化
    if ( pxEnd == NULL ) vMemInit();

    //對齊記憶體;總申請記憶體大小
    if ( ( xTotleAllocSize & VM_ALIGNMENT_MASK ) != 0 ) {
        xTotleAllocSize += ( size_t )( VM_ALIGNMENT_SIZE - 1 );
        xTotleAllocSize &= ~( ( size_t )( VM_ALIGNMENT_MASK ) );
    }

    // 走訪每一個空鏈表，並檢查能容納申請記憶體的最小鏈表
    pPreviousVisitBlock = &pxStart;
    pVisitBlock         = pPreviousVisitBlock->nextBlock;
    while ( pVisitBlock != pxEnd ) {
        if ( pVisitBlock->size >= xTotleAllocSize ) {
            if ( pAllocBlock == NULL || pVisitBlock->size < pAllocBlock->size ) {
                pAllocBlock         = pVisitBlock;
                pPreviousAllocBlock = pPreviousVisitBlock;
            }
        }
        pPreviousVisitBlock = pVisitBlock;
        pVisitBlock         = pVisitBlock->nextBlock;
    }

    //如果找到合適的分配鏈表，則建立新的鏈表，並分配空間
    if ( pAllocBlock != NULL ) {
        pReturn                        = ( void* )( ( size_t )pAllocBlock + xBlockStructSize );
        pNewBlock                      = ( void* )( ( size_t )pAllocBlock + xTotleAllocSize );
        pNewBlock->nextBlock           = pAllocBlock->nextBlock;
        pNewBlock->size                = pAllocBlock->size - xTotleAllocSize;
        pAllocBlock->size              = xTotleAllocSize - xBlockStructSize;
        pPreviousAllocBlock->nextBlock = pNewBlock;
    }
    return pReturn;
}

/**
 * @brief  釋放空間
 * @note   
 * @param  pv: 空間指標
 * @retval None
 */
void vMemFree( void* pv ) {
    stBlock_T* pVisitBlock         = NULL;
    stBlock_T* pPreviousVisitBlock = NULL;
    stBlock_T* pFreeBlock          = ( void* )( ( size_t )pv - xBlockStructSize );

    //走訪鏈表
    pPreviousVisitBlock = &pxStart;
    pVisitBlock         = pPreviousVisitBlock->nextBlock;
    while ( pVisitBlock != NULL ) {
        //尋找大於pv位址的最小位址block
        if ( ( size_t )pVisitBlock > ( size_t )pFreeBlock ) {
            //檢查釋放空間尾部是否剛好連接鏈表，如果連接，將他們合成。
            if ( ( ( size_t )pFreeBlock + xBlockStructSize + pFreeBlock->size ) == ( size_t )pVisitBlock ) {
                pFreeBlock->nextBlock = pVisitBlock->nextBlock;
                pFreeBlock->size += ( xBlockStructSize + ( pVisitBlock->size ) );
            }
            else {
                pFreeBlock->nextBlock = pVisitBlock;
            }

            //檢查釋放空間頭部是否剛好連接鏈表，如果連接，將他們合成。
            if ( ( ( size_t )pPreviousVisitBlock + xBlockStructSize + pPreviousVisitBlock->size ) == ( size_t )pFreeBlock ) {
                pPreviousVisitBlock->nextBlock = pFreeBlock->nextBlock;
                pPreviousVisitBlock->size += ( xBlockStructSize + ( pFreeBlock->size ) );
            }
            else {
                pPreviousVisitBlock->nextBlock = pFreeBlock;
            }
            break;
        }
        pPreviousVisitBlock = pVisitBlock;
        pVisitBlock         = pVisitBlock->nextBlock;
    }
}

void vMemInfoPrint( void ) {
    stBlock_T* pVisitBlock = pxStart.nextBlock;
    size_t     uAddr       = ( size_t )memArr;
    size_t c_memAddr;

    if ( ( uAddr & VM_ALIGNMENT_MASK ) != 0 ) {
        uAddr += ( size_t )( VM_ALIGNMENT_SIZE - 1 );
        uAddr &= ~( ( size_t )( VM_ALIGNMENT_MASK ) );
    }
    printf("---------------------------------------------------\n");
    printf( "Origin Memrary Address : 0x%X ; Size : 0x%X(%d)\n", ( size_t )memArr, VM_MEM_SIZE, VM_MEM_SIZE );
    printf( "Alignment Memrary Address : 0x%X\n", ( size_t )memArr );
    printf( "Start Block Address : 0x%X\n", ( size_t )&pxStart );
    printf( "End Block Address : 0x%X\n", ( size_t )pxEnd );
    printf( "Total Size : 0x%X(%d)\n", ( size_t )pxEnd - ( size_t )memArr, ( size_t )pxEnd - ( size_t )memArr );
    printf("---------------------------------------------------\n");
    pVisitBlock = pxStart.nextBlock;
    while ( pVisitBlock->nextBlock != NULL ){
        printf("Block Address : 0x%X\n", (size_t)pVisitBlock);
        printf("Block Size : 0x%X(%d)\n", pVisitBlock->size, pVisitBlock->size);
        printf("Block Next Link Address : 0x%X\n", (size_t)pVisitBlock->nextBlock);
        printf("---------------------------\n");
        pVisitBlock = pVisitBlock->nextBlock;
    }
    c_memAddr = uAddr;
    pVisitBlock = pxStart.nextBlock;
    while ( pVisitBlock->nextBlock != NULL ) {
        size_t i;
        while(c_memAddr < (size_t)pVisitBlock){
            printf("#");
            c_memAddr++;
        }
        for(i=0;i<xBlockStructSize;i++){
            printf("@");
            c_memAddr++;
        }
        for(i=0;i<pVisitBlock->size;i++){
            printf(":");
            c_memAddr++;
        }
        pVisitBlock = pVisitBlock->nextBlock;
    }
    printf("\n");
}
