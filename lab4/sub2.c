#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>

#define MAX_COUNT 50

void f_stack(int depth) {
    char stack_array[4096]; 
    printf("Depth %d, stack address: %p\n", depth, (void*)&stack_array);
    sleep(1);
    if (depth < MAX_COUNT) {
	f_stack(depth + 1);
    }
}

void sigsegv_handler(int sig) {
    printf("Caught SIGSEGV\n");
    exit(1);
}

int main(int argc, char* argv[]) {
    // 2a
    if (argc == 1) {
        printf("PID: %d\n", getpid());
        sleep(1);
        execv(argv[0], argv);
        printf("Hello world\n");
        return 0;
    } 

    // 2c
    printf("PID: %d\n", getpid());
    printf("Use watch -d cat /proc/%d/maps", getpid());
    sleep(10);
    
    if (argc == 2) {
        printf("\nTesting stack growth:\n");
        f_stack(0);
    }

    // c v
    if (argc == 3) {
        printf("\nTesting heap growth:\n");
        sleep(5);
        void* ptrs[MAX_COUNT];
        for (int i = 0; i < MAX_COUNT; i++) {
            ptrs[i] = malloc(1024 * 4 * i);
            printf("Allocated %dKB at %p\n", 4*i, ptrs[i]);
            sleep(1);
        }

        printf("\nFreeing heap memory:\n");
        sleep(2);
        for (int i = 0; i < MAX_COUNT; i++) {
            free(ptrs[i]);
            printf("Freed %p\n", ptrs[i]);
            sleep(1);
        }
    }

    // c viii
    if (argc == 4) {
	sleep(10);
        int pagesize = getpagesize();
        printf("Page size: %d\n", pagesize);
        char* mapped = mmap(NULL, 10 * pagesize, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
        if (mapped == MAP_FAILED) {
            perror("mmap failed");
            return 1;
        }
        printf("\nMapped 10 pages at %p\n", mapped);
        sleep(10);

    // c x
        signal(SIGSEGV, sigsegv_handler);
        printf("\nSetting region to PROT_NONE...\n");
        mprotect(mapped, 10 * pagesize, PROT_NONE);
        sleep(10);
        //printf("Attempting to read (expect SIGSEGV)...\n");
        //char c = *mapped;
        //printf("Read value: %c\n", c);
        //printf("Attempting to write (expec SIGSEGV)...\n");
        //mapped[10] = 'a';

        printf("\nUnmapping pages 4-6...\n");
        munmap(mapped + 4 * pagesize, 3 * pagesize);
        sleep(10);
    }

    return 0;
}
