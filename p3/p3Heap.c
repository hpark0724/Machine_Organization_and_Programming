///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2021-23 Deb Deppeler
// Posting or sharing this file is prohibited, including any changes/additions.
//
// We have provided comments and structure for this program to help you get 
// started.  Later programs will not provide the same level of commenting,
// rather you will be expected to add same level of comments to your work.
// 09/20/2021 Revised to free memory allocated in get_board_size function.
// 01/24/2022 Revised to use pointers for CLAs
//
////////////////////////////////////////////////////////////////////////////////
// Main File:        p3Heap
// This File:        p3Heap
// Other Files:      None
// Semester:         CS 354 Fall 2023
// Instructor:       Deb Deppeler
//
// Author:           Hye Won Park
// Email:            hpark383
// CS Login:         hyep
// GG#:              1
//                   (See https://canvas.wisc.edu/groups for your GG number)
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   Track all work you do in your work log-p2a.pdf
//                   and fully credit all sources of help, including family, 
//                   friends, tutors, Peer Mentors, TAs, and Instructor.
//
// Online sources:   Avoid relying on eeb searches to solve your problems, 
//                   but if you do search, be sure to include Web URLs and 
//                   description of any information you find in your work log.
////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020-2023 Deb Deppeler based on work by Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission FALL 2023, CS354-deppeler
//
///////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "p3Heap.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block.
 */
typedef struct blockHeader {           

    /*
     * 1) The size of each heap block must be a multiple of 8
     * 2) heap blocks have blockHeaders that contain size and status bits
     * 3) free heap block contain a footer, but we can use the blockHeader 
     *.
     * All heap blocks have a blockHeader with size and status
     * Free heap blocks have a blockHeader as its footer with size only
     *
     * Status is stored using the two least significant bits.
     *   Bit0 => least significant bit, last bit
     *   Bit0 == 0 => free block
     *   Bit0 == 1 => allocated block
     *
     *   Bit1 => second last bit 
     *   Bit1 == 0 => previous block is free
     *   Bit1 == 1 => previous block is allocated
     * 
     * Start Heap: 
     *  The blockHeader for the first block of the heap is after skip 4 bytes.
     *  This ensures alignment requirements can be met.
     * 
     * End Mark: 
     *  The end of the available memory is indicated using a size_status of 1.
     * 
     * Examples:
     * 
     * 1. Allocated block of size 24 bytes:
     *    Allocated Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 25
     *      If the previous block is allocated p-bit=1 size_status would be 27
     * 
     * 2. Free block of size 24 bytes:
     *    Free Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 24
     *      If the previous block is allocated p-bit=1 size_status would be 26
     *    Free Block Footer:
     *      size_status should be 24
     */
    int size_status;

} blockHeader;         

/* Global variable - DO NOT CHANGE NAME or TYPE. 
 * It must point to the first block in the heap and is set by init_heap()
 * i.e., the block at the lowest address.
 */
blockHeader *heap_start = NULL;     

/* Size of heap allocation padded to round to nearest page size.
 */
int alloc_size;

/*
 * Additional global variables may be added as needed below
 * TODO: add global variables needed by your function
 */

blockHeader *checkBlock = NULL; // pointer to the block that is checked in the method balloc() for memory allocation
blockHeader *allocatedBlock = NULL; // pointer to the block in which the memory will be allocated 
                                    // when the free block with suitable size is found for memory allocation
blockHeader *footerBlock = NULL; // pointer to the footer block
blockHeader *splitBlock = NULL; // pointer to the free block in which it is splited after memory allocation


/* 
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block (payload) on success.
 * Returns NULL on failure.
 *
 * This function must:
 * - Check size - Return NULL if size < 1 
 * - Determine block size rounding up to a multiple of 8 
 *   and possibly adding padding as a result.
 *
 * - Use BEST-FIT PLACEMENT POLICY to chose a free block
 *
 * - If the BEST-FIT block that is found is exact size match
 *   - 1. Update all heap blocks as needed for any affected blocks
 *   - 2. Return the address of the allocated block payload
 *
 * - If the BEST-FIT block that is found is large enough to split 
 *   - 1. SPLIT the free block into two valid heap blocks:
 *         1. an allocated block
 *         2. a free block
 *         NOTE: both blocks must meet heap block requirements 
 *       - Update all heap block header(s) and footer(s) 
 *              as needed for any affected blocks.
 *   - 2. Return the address of the allocated block payload
 *
 *   Return if NULL unable to find and allocate block for required size
 *
 * Note: payload address that is returned is NOT the address of the
 *       block header.  It is the address of the start of the 
 *       available memory for the requesterr.
 *
 * Tips: Be careful with pointer arithmetic and scale factors.
 */

    void* balloc(int size) {     
        // return NULL if size < 1 
        if( size <= 0 ){
            return NULL;
        }

        // Determine block size rounding up to a multiple of 8 
        // and possibly adding padding as a result.
        int blockSize = size + sizeof(blockHeader);
        if(blockSize % 8 != 0){
            blockSize = blockSize + (8 - (blockSize % 8)); // add padding to round
                                                        // up to multiple of 8, 
        }

        int checkBlock_size = 0; // initialize the size of the block that we check to allocate the memory

        checkBlock = heap_start; // initialize the free block that we check to allocate the memory
                                // to the start of the heap in the memory
        allocatedBlock = heap_start; // initialize the block that we want to allocate to the heap
                                    // to the start of the heap in the memory
        footerBlock = heap_start; // initialize the block for footer that contains the size of the memory
                                // to the start of the heap in the memory
        splitBlock = heap_start; // initialize the block for free block after the memory has been allocated
                                // and is splited to the start of the heap in the memory

        // loop until it reaches the end mark when unable 
        // to find and allocate block for required size
        while((checkBlock -> size_status) != 1){
            // the actual size of the free block that we want to allocate
            checkBlock_size= checkBlock -> size_status - (checkBlock -> size_status % 8);
            // when the block is free
            if(((checkBlock -> size_status) % 2) == 0) {
                // BEST-FIT block that is found is exact size match
                // check if the block is exactly the size that we want to allocate,
                // allocate the memory to this free block
                if(blockSize == checkBlock_size){
                    // allocate the size
                    allocatedBlock = checkBlock;
                    // if the previous block is allocated
                    if((checkBlock -> size_status % 8) == 2){
                        // update the size status by adding 2 + 1 for 
                        // 1-bit and 0 bit
                        allocatedBlock -> size_status = blockSize + 3;
                    }
                    // if the previous block is free
                    else {
                        // update the size status by adding 1 for 0-bit
                        allocatedBlock -> size_status = blockSize + 1;
                    }
                    // size of the block we allocated
                    int allocatedSize = allocatedBlock -> size_status;
                    // calculate the actual size by subtracting the p-bit from the size status
                    int actualSize = allocatedSize - (allocatedSize % 8);
                    // add the size of the allocated block to go to next block 
                    char* nextBlock = (char*)allocatedBlock + actualSize;
                    blockHeader *next_status = (blockHeader *)(nextBlock);
                    // when the next block is free 
                    if((next_status -> size_status) != 1){
                        // add 2 since the previous block has been allocated
                        next_status -> size_status = next_status -> size_status + 2;
                    }
                    // return the address of the allocated block payload
                    return (char*)allocatedBlock + sizeof(blockHeader);
                }

                else{
                    // BEST-FIT block that is found is large enough to split 
                    // when the block is larger than the size that we want to allocate
                    if(blockSize < checkBlock_size){
                        // allocate the block
                        allocatedBlock = checkBlock;
                        // if the previous block is allocated
                        if(((checkBlock -> size_status) % 8) == 2){
                            // update the size status by adding 2 + 1 for 
                            // 1-bit and 0-bit
                            allocatedBlock -> size_status = blockSize + 3;
                        }
                        // if the previous block is free
                        else{
                            // update the size status by adding 1 for 0-bit
                            allocatedBlock -> size_status = blockSize + 1;
                        }

                        
                        // split the block 
                        splitBlock = (blockHeader*)((char*)allocatedBlock + blockSize);
                        // split the block by subtrating the blocksize from the size of  
                        // free block and add p-bit since the previous block is allocated
                        splitBlock -> size_status = checkBlock_size - blockSize + 2;

                        // size of the free block we splited
                        int splitBlockSize = splitBlock -> size_status;
                        // calculate the actual splited free block's size by subtracting the p-bit from the size status
                        int actualSplitSize = splitBlockSize - (splitBlockSize % 8);
 
                        // add the free block which is splited and move back by 4 byte to 
                        // move to footer 
                        char* footer_Block_caster = (char*)splitBlock+ actualSplitSize - 4;
                        // change the type to blockHeader since the footerBlock is blockHeader struct
                        // variable 
                        footerBlock = (blockHeader *)(footer_Block_caster);
                        // update the free splited block's footer
                        footerBlock -> size_status = actualSplitSize;
                        //return the address of the allocated block payload
                        return (char*)allocatedBlock + sizeof(blockHeader);
                    }
                }
            }

            // when the block is not free or 
            // when the the block is free but it is not larger or same as the block that we want to allocate
            
            // the actual size of the free block that we want to allocate
            checkBlock_size= checkBlock -> size_status - (checkBlock -> size_status % 8);
            // move to the next block to check if the next block is free and iterate
            // the conditions to allocate the block
            checkBlock = (blockHeader*)((char*)checkBlock + checkBlock_size);
        }
        // return NULL when the blocks are all iterated to be checked for memory interation
        // and there is no free block that can be allocated
        return NULL;
    } 




/* 
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - Update header(s) and footer as needed.
 *
 * If free results in two or more adjacent free blocks,
 * they will be immediately coalesced into one larger free block.
 * so free blocks require a footer (blockHeader works) to store the size
 *
 * TIP: work on getting immediate coalescing to work after your code 
 *      can pass the tests in partA and partB of tests/ directory.
 *      Submit code that passes partA and partB to Canvas before continuing.
 */                    
    int bfree(void *ptr) {    
        //TODO: Your code goes in here.

        // initialize the start address(before header) of the ptr block 
        blockHeader* ptr_start = (blockHeader*)((char*)ptr - sizeof(blockHeader));
        // the start address(before header) of block after ptr block
        blockHeader* next_ptr_start = heap_start;
        // the start address(before header) of the block before ptr block
        blockHeader* prev_ptr_start = heap_start;
        // the end address of the heap space
        blockHeader* heap_end = (blockHeader*)((char*)heap_start + alloc_size);

        // initialize the address of the footer in the ptr block
        blockHeader* ptr_footer = heap_start;
        // initialize the address of the footer in the previous block of ptr block
        blockHeader* prev_footer = heap_start;
        // initialize the address of the footer in the next block ptr block
        blockHeader* next_footer = heap_start;

        // the actual size of ptr without p-bit and a-bit
        int actualSize_ptr = 0;
        // the actual size of previous block of ptr block without p-bit and a-bit
        int actualSize_prev = 0;
        // the actual size of next block of ptr block without p-bit and a-bit
        int actualSize_next = 0;




        // return -1 if ptr is NULL
        if(ptr == NULL){
            return -1;
        }
        // return -1 if ptr is not a multiple of 8
        else if((unsigned int)ptr % 8 != 0){
            return -1;
        }
        // return -1 if ptr is outside of the heap space
        else if((ptr_start < heap_start || (blockHeader*)((char*)ptr_start + sizeof(blockHeader)) > heap_end)){
            return -1;
        }
        // return -1 if ptr block is already freed, (if the current block is allocated, 
        // a-bit is 1 so the size_status is always odd, else it is free)
        else if(ptr_start -> size_status % 2 != 1){
            return -1;
        }

        else{
            // store actual size of ptr (remove p-bit and a-bit)
            actualSize_ptr = ptr_start -> size_status - (ptr_start -> size_status % 8);
            // update the start address (before header) of next block of ptr block
            next_ptr_start = (blockHeader*)((char*)ptr_start + actualSize_ptr);

            // step 1: free the block at ptr
            
            // change a-bit from 1 to 0 (the ptr block a-bit) to free the ptr block
            ptr_start -> size_status = ptr_start -> size_status - 1;
            if(next_ptr_start -> size_status != 1){
                // change the p-bit from 2 to 0 (the block after ptr block p-bit)
                next_ptr_start -> size_status = next_ptr_start -> size_status - 2;
            }

            // update the footer position of ptr block
            ptr_footer = (blockHeader*)((char*)next_ptr_start - sizeof(blockHeader));
            // update the size of footer
            ptr_footer -> size_status = actualSize_ptr;

            // p-bit of ptr block, we can know if the ptr's previous block 
            // is free or not with it since we know that ptr is free and its a-bit is 0
            int ptr_pbit_status = ptr_start -> size_status % 8;

            // a-bit of ptr block, we can know if the block next to ptr block is free or not
            // since we know that ptr is free and its p-bit is 0
            int next_ptr_status = next_ptr_start -> size_status % 8;

            // store actual size of next block (remove p-bit and a-bit)
            actualSize_next = next_ptr_start -> size_status - next_ptr_status;
            // store actual size of previous block (remove p-bit and a-bit)
            actualSize_prev = prev_ptr_start -> size_status - (prev_ptr_start -> size_status % 8);

            // step 2: immediate coalescing  

            // step2 - 1: coalesce left
            // if the previous block of ptr is free, coalesce with previous block first
            if(ptr_pbit_status == 0){ 
                // footer of previous block address
                prev_footer = (blockHeader*)((char*)ptr_start - sizeof(blockHeader));

                // update the footer size by adding the ptr block size and previous block size
                ptr_footer -> size_status = actualSize_ptr + actualSize_prev;

                // header of the previous block of ptr
                prev_ptr_start = (blockHeader*)((char*)ptr_start - prev_footer -> size_status);
                // update the size of the coalesced block after coalescing left
                prev_ptr_start -> size_status = prev_ptr_start -> size_status + actualSize_ptr; 

                // step 2 - 2: coalesce right with the free block that is previously coalesced left
                // if the next block is free while the previous block was freed and coalesced
                if(next_ptr_status ==0){
                    // the footer of next block of ptr
                    next_footer = (blockHeader*)((char*)next_ptr_start + actualSize_next - sizeof(blockHeader));
                    // update the footer size after coalescing right by adding the actual size of ptr block and 
                    // the actual size of next block of ptr 
                    next_footer -> size_status = actualSize_next + actualSize_ptr;
                    // update the header after coalescing right by adding the size of 
                    // previous free block (coalesced size of previous and ptr bloce)
                    // and the size of next free block
                    prev_ptr_start -> size_status = prev_ptr_start -> size_status + actualSize_next;

                }  
                
            }

            // step 2- 3: coalesce right
            // if the next block is free and the previous block is allocated
            // or the ptr block is start of the heap
            else if(next_ptr_status ==0 && ptr_pbit_status == 2){
                // the footer in the next block of ptr
                next_footer = (blockHeader*)((char*)next_ptr_start + actualSize_next - sizeof(blockHeader));
                // update the footer size of next block for coalescing right
                next_footer -> size_status = next_footer -> size_status + actualSize_ptr;
                // update the ptr block header size by adding the next block size to the ptr block size               
                ptr_start -> size_status = ptr_start -> size_status + actualSize_next;
            }
            // returns 0 on success
            return 0;
        }

    } 


/* 
 * Initializes the memory allocator.
 * Called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */                    
int init_heap(int sizeOfRegion) {    

    static int allocated_once = 0; //prevent multiple myInit calls

    int   pagesize; // page size
    int   padsize;  // size of padding when heap size not a multiple of page size
    void* mmap_ptr; // pointer to memory mapped area
    int   fd;

    blockHeader* end_mark;

    if (0 != allocated_once) {
        fprintf(stderr, 
                "Error:mem.c: InitHeap has allocated space during a previous call\n");
        return -1;
    }

    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize from O.S. 
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    mmap_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == mmap_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }

    allocated_once = 1;

    // for double word alignment and end mark
    alloc_size -= 8;

    // Initially there is only one big free block in the heap.
    // Skip first 4 bytes for double word alignment requirement.
    heap_start = (blockHeader*) mmap_ptr + 1;

    // Set the end mark
    end_mark = (blockHeader*)((void*)heap_start + alloc_size);
    end_mark->size_status = 1;

    // Set size in header
    heap_start->size_status = alloc_size;

    // Set p-bit as allocated in header
    // note a-bit left at 0 for free
    heap_start->size_status += 2;

    // Set the footer
    blockHeader *footer = (blockHeader*) ((void*)heap_start + alloc_size - 4);
    footer->size_status = alloc_size;

    return 0;
} 

/* STUDENTS MAY EDIT THIS FUNCTION, but do not change function header.
 * TIP: review this implementation to see one way to traverse through
 *      the blocks in the heap.
 *
 * Can be used for DEBUGGING to help you visualize your heap structure.
 * It traverses heap blocks and prints info about each block found.
 * 
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block 
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block as stored in the block header
 */                     
void disp_heap() {     

    int    counter;
    char   status[6];
    char   p_status[6];
    char * t_begin = NULL;
    char * t_end   = NULL;
    int    t_size;

    blockHeader *current = heap_start;
    counter = 1;

    int used_size =  0;
    int free_size =  0;
    int is_used   = -1;

    fprintf(stdout, 
            "*********************************** HEAP: Block List ****************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, 
            "---------------------------------------------------------------------------------\n");

    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;

        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "alloc");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "FREE ");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "alloc");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "FREE ");
        }

        if (is_used) 
            used_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;

        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%4i\n", counter, status, 
                p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);

        current = (blockHeader*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, 
            "---------------------------------------------------------------------------------\n");
    fprintf(stdout, 
            "*********************************************************************************\n");
    fprintf(stdout, "Total used size = %4d\n", used_size);
    fprintf(stdout, "Total free size = %4d\n", free_size);
    fprintf(stdout, "Total size      = %4d\n", used_size + free_size);
    fprintf(stdout, 
            "*********************************************************************************\n");
    fflush(stdout);

    return;  
} 


// end p3Heap.c (Fall 2023)                                         

