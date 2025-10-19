// SPDX-License-Identifier: GPL-2.0
/*
 * Cognitive kernel debugging interfaces
 * 
 * Provides debugging and introspection capabilities for the
 * cognitive kernel subsystem
 */

#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/cognitive/atomspace.h>
#include <linux/cognitive/sched.h>

static struct proc_dir_entry *cognitive_proc_dir;

/**
 * cognitive_atomspace_show - Show AtomSpace statistics
 */
static int cognitive_atomspace_show(struct seq_file *m, void *v)
{
    seq_printf(m, "AtomSpace Statistics:\n");
    seq_printf(m, "Total atoms: %d\n", atomic_read(&kernel_atomspace.atom_count));
    seq_printf(m, "Next UUID: %llu\n", atomic64_read(&kernel_atomspace.next_uuid));
    
    return 0;
}

/**
 * cognitive_scheduler_show - Show cognitive scheduler statistics
 */
static int cognitive_scheduler_show(struct seq_file *m, void *v)
{
    int cpu;
    
    seq_printf(m, "Cognitive Scheduler Statistics:\n");
    seq_printf(m, "Policy: %d\n", cognitive_sched_get_policy());
    seq_printf(m, "Attention budget: %u\n", cognitive_sched_config.attention_budget);
    seq_printf(m, "Learning boost: %u\n", cognitive_sched_config.learning_boost);
    
    seq_printf(m, "\nPer-CPU Statistics:\n");
    for_each_online_cpu(cpu) {
        u32 available = cognitive_get_available_attention(cpu);
        seq_printf(m, "CPU %d: Available attention: %u\n", cpu, available);
    }
    
    return 0;
}

/**
 * cognitive_tasks_show - Show cognitive tasks information
 */
static int cognitive_tasks_show(struct seq_file *m, void *v)
{
    struct task_struct *task;
    struct cognitive_task *attrs;
    
    seq_printf(m, "Cognitive Tasks:\n");
    seq_printf(m, "PID\tComm\t\tReasoning\tAttention\tGoal\n");
    
    rcu_read_lock();
    for_each_process(task) {
        attrs = task_get_cognitive_attrs(task);
        if (attrs) {
            seq_printf(m, "%d\t%-15s\t%u\t\t%u\t\t%s\n",
                      task->pid, task->comm,
                      attrs->reasoning_load,
                      attrs->attention_value,
                      attrs->current_goal ? "Yes" : "No");
        }
    }
    rcu_read_unlock();
    
    return 0;
}

/* Proc file operations */
DEFINE_PROC_SHOW_ATTRIBUTE(cognitive_atomspace);
DEFINE_PROC_SHOW_ATTRIBUTE(cognitive_scheduler);
DEFINE_PROC_SHOW_ATTRIBUTE(cognitive_tasks);

/**
 * cognitive_debug_init - Initialize debugging interfaces
 */
int __init cognitive_debug_init(void)
{
    /* Create /proc/cognitive directory */
    cognitive_proc_dir = proc_mkdir("cognitive", NULL);
    if (!cognitive_proc_dir) {
        pr_err("cognitive: Failed to create /proc/cognitive\n");
        return -ENOMEM;
    }
    
    /* Create AtomSpace info file */
    if (!proc_create("atomspace", 0444, cognitive_proc_dir, 
                     &cognitive_atomspace_proc_ops)) {
        pr_err("cognitive: Failed to create atomspace proc entry\n");
        goto err_atomspace;
    }
    
    /* Create scheduler info file */
    if (!proc_create("scheduler", 0444, cognitive_proc_dir,
                     &cognitive_scheduler_proc_ops)) {
        pr_err("cognitive: Failed to create scheduler proc entry\n");
        goto err_scheduler;
    }
    
    /* Create tasks info file */
    if (!proc_create("tasks", 0444, cognitive_proc_dir,
                     &cognitive_tasks_proc_ops)) {
        pr_err("cognitive: Failed to create tasks proc entry\n");
        goto err_tasks;
    }
    
    pr_info("cognitive: Debug interfaces initialized\n");
    return 0;

err_tasks:
    remove_proc_entry("scheduler", cognitive_proc_dir);
err_scheduler:
    remove_proc_entry("atomspace", cognitive_proc_dir);
err_atomspace:
    remove_proc_entry("cognitive", NULL);
    return -ENOMEM;
}

/**
 * cognitive_debug_cleanup - Cleanup debugging interfaces
 */
void cognitive_debug_cleanup(void)
{
    if (cognitive_proc_dir) {
        remove_proc_entry("tasks", cognitive_proc_dir);
        remove_proc_entry("scheduler", cognitive_proc_dir);
        remove_proc_entry("atomspace", cognitive_proc_dir);
        remove_proc_entry("cognitive", NULL);
    }
    
    pr_info("cognitive: Debug interfaces cleaned up\n");
}

EXPORT_SYMBOL(cognitive_debug_init);
EXPORT_SYMBOL(cognitive_debug_cleanup);