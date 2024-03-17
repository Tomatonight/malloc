#pragma once
#include<unistd.h>
#include<sys/mman.h>
#define PG_SIZE 4096
#define SBRK_ONE_TIME 4096*32
#define BLOCKS_SIZE 8
#define MAGIC_NUMBER 0x12341234
struct block_info
{
block_info* next;
unsigned int size;
int magic_number;      
}__attribute__((packed));//16bytes

struct mem_pool  //32 64 128 256 512 1024 2048 4096
{
void* mem_start;
int left_size;
block_info* blocks[BLOCKS_SIZE];
};
mem_pool mempool;
void* malloc_(int size);
void free_(void*);
static void add_block(block_info* &blocks,block_info* block);
static block_info* remove_block(block_info* &blocks);