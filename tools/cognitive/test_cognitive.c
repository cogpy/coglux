/*
 * Test program for OpenCog AGI-OS cognitive kernel
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

/* Cognitive system call numbers */
#define __NR_cognitive_base     450
#define __NR_cognitive_get_attention    (__NR_cognitive_base + 4)

int main(int argc, char *argv[])
{
    printf("OpenCog AGI-OS Cognitive Kernel Test Program\n");
    printf("===========================================\n\n");
    
    /* Check if cognitive kernel is available */
    long test_attention = syscall(__NR_cognitive_get_attention);
    if (test_attention < 0 && errno == ENOSYS) {
        printf("✗ Cognitive kernel system calls not available\n");
        printf("  Make sure the cognitive kernel is compiled and loaded\n");
        return 1;
    }
    
    printf("✓ Cognitive kernel functionality available\n");
    printf("✓ Available attention units: %ld\n", test_attention);
    
    return 0;
}