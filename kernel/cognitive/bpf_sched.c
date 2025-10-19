// SPDX-License-Identifier: GPL-2.0
/*
 * BPF-based cognitive scheduler extension
 * 
 * Integrates with sched_ext to provide cognitive-aware scheduling
 * using BPF programs for dynamic behavior modification
 */

#include <linux/bpf.h>
#include <linux/sched/ext.h>
#include <linux/cognitive/sched.h>
#include <linux/cognitive/atomspace.h>

#ifdef CONFIG_SCHED_CLASS_EXT

/* BPF cognitive scheduler operations */
struct sched_ext_ops cognitive_sched_ops;

/**
 * cognitive_bpf_select_cpu - Select CPU for cognitive task
 * @p: Task to schedule
 * @prev_cpu: Previous CPU
 * @wake_flags: Wake flags
 * 
 * Returns: Selected CPU or negative for default selection
 */
static s32 cognitive_bpf_select_cpu(struct task_struct *p, s32 prev_cpu, u64 wake_flags)
{
    struct cognitive_task *attrs;
    int cpu;
    u32 best_attention = 0;
    s32 best_cpu = -1;
    
    /* Check if this is a cognitive task */
    attrs = task_get_cognitive_attrs(p);
    if (!attrs) {
        return -1;  /* Use default CPU selection */
    }
    
    /* Find CPU with most available attention for cognitive tasks */
    for_each_online_cpu(cpu) {
        u32 available = cognitive_get_available_attention(cpu);
        
        /* Prefer CPUs with sufficient attention for this task */
        if (available >= attrs->attention_value && available > best_attention) {
            best_attention = available;
            best_cpu = cpu;
        }
    }
    
    return best_cpu != -1 ? best_cpu : prev_cpu;
}

/**
 * cognitive_bpf_enqueue - Enqueue cognitive task
 * @p: Task to enqueue
 * @enq_flags: Enqueue flags
 */
static void cognitive_bpf_enqueue(struct task_struct *p, u64 enq_flags)
{
    struct cognitive_task *attrs;
    
    attrs = task_get_cognitive_attrs(p);
    if (!attrs) {
        /* Not a cognitive task, use default scheduling */
        scx_bpf_dispatch(p, SCX_DSQ_GLOBAL, 0, enq_flags);
        return;
    }
    
    /* Allocate attention for the task */
    if (cognitive_allocate_attention(p, attrs->attention_value) == 0) {
        /* Successfully allocated attention, dispatch with priority */
        u64 slice_ns = attrs->goal_priority * 1000000;  /* Priority-based time slice */
        scx_bpf_dispatch(p, SCX_DSQ_GLOBAL, slice_ns, enq_flags);
    } else {
        /* Insufficient attention, lower priority */
        scx_bpf_dispatch(p, SCX_DSQ_GLOBAL, 0, enq_flags);
    }
}

/**
 * cognitive_bpf_dequeue - Dequeue cognitive task
 * @p: Task to dequeue
 */
static void cognitive_bpf_dequeue(struct task_struct *p, u64 deq_flags)
{
    struct cognitive_task *attrs;
    
    attrs = task_get_cognitive_attrs(p);
    if (attrs) {
        /* Release attention when task stops running */
        cognitive_release_attention(p, attrs->attention_value / 2);
    }
}

/**
 * cognitive_bpf_running - Task started running
 * @p: Task that started running
 */
static void cognitive_bpf_running(struct task_struct *p)
{
    struct cognitive_task *attrs;
    
    attrs = task_get_cognitive_attrs(p);
    if (attrs) {
        /* Update focus tracking */
        attrs->focus_duration = ktime_get_ns();
        
        /* Record learning event if this is a learning task */
        if (attrs->learning_rate > 50) {
            cognitive_record_learning_event(p, attrs->learning_rate);
        }
    }
}

/**
 * cognitive_bpf_stopping - Task stopping
 * @p: Task that is stopping
 * @runnable: Whether task remains runnable
 */
static void cognitive_bpf_stopping(struct task_struct *p, bool runnable)
{
    struct cognitive_task *attrs;
    u64 runtime_ns;
    
    attrs = task_get_cognitive_attrs(p);
    if (attrs) {
        /* Calculate runtime for focus tracking */
        runtime_ns = ktime_get_ns() - attrs->focus_duration;
        
        /* Update adaptation score based on runtime efficiency */
        if (runtime_ns > 0) {
            u32 adaptation_score = min(100U, (u32)(runtime_ns / 1000000));  /* Convert to ms */
            cognitive_update_adaptation_score(p, adaptation_score);
        }
    }
}

/**
 * cognitive_bpf_init - Initialize cognitive BPF scheduler
 */
static s32 cognitive_bpf_init(struct sched_ext_ops *ops)
{
    pr_info("cognitive: BPF cognitive scheduler initialized\n");
    return 0;
}

/**
 * cognitive_bpf_exit - Cleanup cognitive BPF scheduler
 */
static void cognitive_bpf_exit(struct sched_ext_ops *ops)
{
    pr_info("cognitive: BPF cognitive scheduler exiting\n");
}

/* Define the sched_ext operations */
static struct sched_ext_ops cognitive_sched_ops = {
    .select_cpu = cognitive_bpf_select_cpu,
    .enqueue = cognitive_bpf_enqueue,
    .dequeue = cognitive_bpf_dequeue,
    .running = cognitive_bpf_running,
    .stopping = cognitive_bpf_stopping,
    .init = cognitive_bpf_init,
    .exit = cognitive_bpf_exit,
    .name = "cognitive"
};

/**
 * cognitive_bpf_sched_init - Initialize BPF cognitive scheduler
 */
int __init cognitive_bpf_sched_init(void)
{
    int ret;
    
    ret = scx_bpf_register(&cognitive_sched_ops);
    if (ret) {
        pr_err("cognitive: Failed to register BPF scheduler: %d\n", ret);
        return ret;
    }
    
    pr_info("cognitive: BPF cognitive scheduler registered\n");
    return 0;
}

/**
 * cognitive_bpf_sched_cleanup - Cleanup BPF cognitive scheduler
 */
void cognitive_bpf_sched_cleanup(void)
{
    scx_bpf_unregister(&cognitive_sched_ops);
    pr_info("cognitive: BPF cognitive scheduler unregistered\n");
}

#else

/* Stub functions when sched_ext is not available */
int __init cognitive_bpf_sched_init(void)
{
    pr_info("cognitive: BPF scheduler not available (sched_ext disabled)\n");
    return 0;
}

void cognitive_bpf_sched_cleanup(void)
{
    /* No-op */
}

#endif /* CONFIG_SCHED_CLASS_EXT */

EXPORT_SYMBOL(cognitive_bpf_sched_init);
EXPORT_SYMBOL(cognitive_bpf_sched_cleanup);