#include "malloc.h"
#include<stdio.h>
#include <unistd.h>
#include<memory.h>

void *malloc_(int size)
{
    size = (((size - 1) >> 3) << 3) + 8;
    if (size > PG_SIZE)
    {
        int page_number = (size + sizeof(block_info) - 1) / PG_SIZE + 1;
        void *re = mmap(NULL, PG_SIZE * page_number, PROT_READ | PROT_WRITE,
                        MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
        block_info *block = (block_info *)re;
        block->next = NULL;
        block->size = page_number*PG_SIZE-sizeof(block_info);
        block->magic_number=MAGIC_NUMBER;
        return (void *)(block + 1);
    }
    else
    {
        block_info *block;
        int index = 0;
        int block_size = 32;
        for (; block_size <= PG_SIZE; block_size *= 2, index++)
        {
            if (block_size >= size)
            {
                break;
            }
        }
        if (!mempool.blocks[index])
        {
            int total_size = sizeof(block_info) + size;
            if (mempool.left_size < total_size)
            {
                for (int cnt = BLOCKS_SIZE - 2, t = PG_SIZE / 2; cnt >= 0; t /= 2, cnt--)
                {
                    int block_size = size + sizeof(block_info);
                    if (mempool.left_size >= block_size)
                    {
                        block_info *temp = (block_info *)mempool.mem_start;
                        mempool.left_size -= block_size;
                        (mempool.mem_start) = (char *)(mempool.mem_start) + block_size;
                        temp->next=NULL;
                        temp->magic_number=MAGIC_NUMBER;
                        temp->size=t;
                        if(mempool.blocks[cnt])
                        add_block(mempool.blocks[cnt], temp);
                        else mempool.blocks[cnt]=temp;
                    }
                }
                for (int cnt = index + 1; cnt < BLOCKS_SIZE; cnt++)
                {
                    if (mempool.blocks[cnt])
                    {
                        mempool.blocks[index] = remove_block(mempool.blocks[cnt]);
                    }
                }

                if (!mempool.blocks[index])
                    mempool.mem_start = sbrk(SBRK_ONE_TIME);
                if ((mempool.mem_start) == (void*)-1)
                    return NULL;
                mempool.left_size = SBRK_ONE_TIME;
            }
            block_info *ret_block = (block_info *)mempool.mem_start;
            mempool.mem_start = (char *)mempool.mem_start + total_size;
            mempool.left_size -= total_size;
            ret_block->size=block_size;
            ret_block->next=NULL;
            ret_block->magic_number=MAGIC_NUMBER;
            mempool.blocks[index]=ret_block;
        }
        block = mempool.blocks[index];
        mempool.blocks[index] = block->next;
        block->next=NULL;
        block->size = block_size;
        return (void *)(block + 1);
    }
}
void free_(void *ptr)
{
    block_info *block = (block_info *)((char *)ptr - sizeof(block_info));
    if(block->magic_number!=MAGIC_NUMBER)
    {
        printf("free err\n");
        return;
    }
    if(block->size>PG_SIZE)
    {
        int re=munmap(block,block->size);
        if(re<0)
        {
            printf("free err\n");
        }
    }
    else
    {
        int cnt=0;
        for(int size=32;size<block->size&&cnt<BLOCKS_SIZE;cnt++,size*=2){}
        if(mempool.blocks[cnt])
        add_block(mempool.blocks[cnt],block);
        else mempool.blocks[cnt]=block;
    }
}

static void add_block(block_info *&blocks, block_info *block)
{
    if (!blocks)
    {
        blocks = block;
        block->next = NULL;
    }
    block_info *temp = blocks;
    block_info *temp_next = blocks->next;
    while (temp_next)
    {
        temp = temp->next;
        temp_next = temp_next->next;
    }
    block->next = NULL;
    temp->next = block;

}
static block_info *remove_block(block_info *&blocks)
{
    block_info *block = blocks;
    blocks = blocks->next;
    block->next = NULL;
    return block;
}