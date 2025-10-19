// SPDX-License-Identifier: GPL-2.0
/*
 * Core cognitive kernel subsystem initialization
 * 
 * This file initializes the OpenCog-based AGI-OS cognitive kernel
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/cognitive/atomspace.h>
#include <linux/cognitive/sched.h>

/* Forward declarations for optional subsystems */
#ifdef CONFIG_COGNITIVE_ATOMSPACE
extern int pattern_engine_init(void);
extern void pattern_engine_cleanup(void);
#endif
#ifdef CONFIG_COGNITIVE_MEMORY
extern int cognitive_memory_init(void);
extern void cognitive_memory_cleanup(void);
#endif

#ifdef CONFIG_COGNITIVE_SCHEDULER
extern int cognitive_sched_init(void);
extern void cognitive_sched_cleanup(void);
extern int cognitive_bpf_sched_init(void);
extern void cognitive_bpf_sched_cleanup(void);
#endif

#ifdef CONFIG_COGNITIVE_DEBUG
extern int cognitive_debug_init(void);
extern void cognitive_debug_cleanup(void);
#endif

/**
 * cognitive_init - Initialize the cognitive kernel subsystem
 */
static int __init cognitive_init(void)
{
    int ret;
    
    pr_info("cognitive: Initializing OpenCog AGI-OS cognitive kernel\n");
    
    /* Initialize AtomSpace if enabled */
#ifdef CONFIG_COGNITIVE_ATOMSPACE
    ret = atomspace_init();
    if (ret) {
        pr_err("cognitive: Failed to initialize AtomSpace: %d\n", ret);
        return ret;
    }
    
    /* Initialize pattern engine */
    ret = pattern_engine_init();
    if (ret) {
        pr_err("cognitive: Failed to initialize pattern engine: %d\n", ret);
        atomspace_cleanup();
        return ret;
    }
#endif

    /* Initialize cognitive scheduler if enabled */
#ifdef CONFIG_COGNITIVE_SCHEDULER
    ret = cognitive_sched_init();
    if (ret) {
        pr_err("cognitive: Failed to initialize cognitive scheduler: %d\n", ret);
        goto err_sched;
    }
    
    /* Initialize BPF cognitive scheduler */
    ret = cognitive_bpf_sched_init();
    if (ret) {
        pr_err("cognitive: Failed to initialize BPF scheduler: %d\n", ret);
        cognitive_sched_cleanup();
        goto err_sched;
    }
#endif

    /* Initialize cognitive memory manager if enabled */
#ifdef CONFIG_COGNITIVE_MEMORY
    ret = cognitive_memory_init();
    if (ret) {
        pr_err("cognitive: Failed to initialize memory manager: %d\n", ret);
        goto err_memory;
    }
#endif

    /* Initialize debug interfaces if enabled */
#ifdef CONFIG_COGNITIVE_DEBUG
    ret = cognitive_debug_init();
    if (ret) {
        pr_err("cognitive: Failed to initialize debug interfaces: %d\n", ret);
        goto err_debug;
    }
#endif

    pr_info("cognitive: AGI-OS cognitive kernel initialized successfully\n");
    return 0;

#ifdef CONFIG_COGNITIVE_DEBUG
err_debug:
#ifdef CONFIG_COGNITIVE_MEMORY
    cognitive_memory_cleanup();
#endif
#endif
#ifdef CONFIG_COGNITIVE_MEMORY
err_memory:
#ifdef CONFIG_COGNITIVE_SCHEDULER
    cognitive_sched_cleanup();
#endif
#endif
#ifdef CONFIG_COGNITIVE_SCHEDULER
err_sched:
#ifdef CONFIG_COGNITIVE_ATOMSPACE
    pattern_engine_cleanup();
    atomspace_cleanup();
#endif
#endif
    return ret;
}

/**
 * cognitive_exit - Cleanup the cognitive kernel subsystem
 */
static void __exit cognitive_exit(void)
{
    pr_info("cognitive: Shutting down cognitive kernel\n");
    
#ifdef CONFIG_COGNITIVE_DEBUG
    cognitive_debug_cleanup();
#endif

#ifdef CONFIG_COGNITIVE_MEMORY
    cognitive_memory_cleanup();
#endif

#ifdef CONFIG_COGNITIVE_SCHEDULER
    cognitive_bpf_sched_cleanup();
    cognitive_sched_cleanup();
#endif

#ifdef CONFIG_COGNITIVE_ATOMSPACE
    pattern_engine_cleanup();
    atomspace_cleanup();
#endif

    pr_info("cognitive: Cognitive kernel shutdown complete\n");
}

subsys_initcall(cognitive_init);
module_exit(cognitive_exit);

MODULE_DESCRIPTION("OpenCog AGI-OS Cognitive Kernel");
MODULE_AUTHOR("CogLux Project");
MODULE_LICENSE("GPL v2");