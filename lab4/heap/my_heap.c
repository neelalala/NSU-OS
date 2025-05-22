#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

#define HEAP_FILE "./heap.txt"
#define HEAP_SIZE (1024 * 1024)

typedef struct BlockHeader {
    size_t size;
    int free;
    struct BlockHeader* next;
    struct BlockHeader* prev;
} BlockHeader;

static void* heap_start = NULL;
static BlockHeader* free_list = NULL;
static int heap_fd = -1;

void init_heap() {
    heap_fd = open(HEAP_FILE, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (heap_fd < 0) {
        perror("open");
        exit(1);
    }

    if (ftruncate(heap_fd, HEAP_SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    heap_start = mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, heap_fd, 0);
    if (heap_start == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    free_list = (BlockHeader*)heap_start;
    free_list->size = HEAP_SIZE - sizeof(BlockHeader);
    free_list->free = 1;
    free_list->next = NULL;
    free_list->prev = NULL;
}

void* my_malloc(size_t size) {
    BlockHeader* current = free_list;

    while (current != NULL) {
        if (current->free && current->size >= size) {
            if (current->size > size + sizeof(BlockHeader)) {
                BlockHeader* new_block = (BlockHeader*)((char*)current + sizeof(BlockHeader) + size);
                new_block->size = current->size - size - sizeof(BlockHeader);
                new_block->free = 1;
                new_block->next = current->next;
                new_block->prev = current;

                current->size = size;
                current->next = new_block;
            }

            current->free = 0;
            return (char*)current + sizeof(BlockHeader);
        }
        current = current->next;
    }

    return NULL;
}

void merge(BlockHeader* block) {
    if (block->prev && block->prev->free) {
        block->prev->size += sizeof(BlockHeader) + block->size;
        block->prev->next = block->next;
        if (block->next)
            block->next->prev = block->prev;
        block = block->prev;
    }

    if (block->next && block->next->free) {
        block->size += sizeof(BlockHeader) + block->next->size;
        block->next = block->next->next;
        if (block->next)
            block->next->prev = block;
    }
}

void my_free(void* ptr) {
    if (ptr == NULL) return;
    BlockHeader* block = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
    block->free = 1;
    merge(block);
}

void close_heap() {
    if (heap_start) {
        munmap(heap_start, HEAP_SIZE);
    }
    if (heap_fd >= 0) {
        close(heap_fd);
    }
}

int main() {
    init_heap();

    sleep(4);
    void* p1 = my_malloc(100);
    sleep(2);
    void* p2 = my_malloc(200);
    strcpy((char*)p1, "Hello Heap 1");
    sleep(2);
    strcpy((char*)p2, "Another block");
    sleep(2);
    my_free(p1);
    sleep(2);
    my_free(p2);
    sleep(2);
    close_heap();
    return 0;
}

