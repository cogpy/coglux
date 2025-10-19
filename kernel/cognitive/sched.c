// SPDX-License-Identifier: GPL-2.0
/*
 * Cognitive scheduler for AGI-OS
 * 
 * Implements cognitive-aware scheduling using OpenCog principles
 */

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/cognitive/sched.h>
#include <linux/cognitive/atomspace.h>

/* Global cognitive scheduler configuration */
struct cognitive_config cognitive_sched_config = {
    .policy = COGNITIVE_POLICY_BALANCED,
    .attention_budget = 1000,
    .learning_boost = 20,
    .reasoning_threshold = 50,
    .enable_goal_scheduling = true,
};

/* Per-CPU cognitive run queues */
static DEFINE_PER_CPU(struct cognitive_rq, cognitive_rq);

/* Memory cache for cognitive task attributes */
static struct kmem_cache *cognitive_task_cache;

/**
 * cognitive_sched_init - Initialize cognitive scheduler
 */
int __init cognitive_sched_init(void)
{
    int cpu;
    
    /* Create memory cache for cognitive task attributes */
    cognitive_task_cache = kmem_cache_create("cognitive_tasks",
                                           sizeof(struct cognitive_task),
                                           0, SLAB_HWCACHE_ALIGN, NULL);
    if (!cognitive_task_cache) {
        pr_err("cognitive: Failed to create task cache\n");
        return -ENOMEM;
    }
    
    /* Initialize per-CPU cognitive run queues */
    for_each_possible_cpu(cpu) {
        struct cognitive_rq *crq = &per_cpu(cognitive_rq, cpu);
        
        crq->cognitive_tasks = RB_ROOT;
        crq->total_attention = cognitive_sched_config.attention_budget;
        crq->available_attention = cognitive_sched_config.attention_budget;
        crq->active_goals = NULL;
        crq->goal_completion_rate = 0;
        crq->learning_cycles = 0;
        crq->avg_adaptation_score = 50;  /* Start with neutral score */
    }
    
    pr_info("cognitive: Cognitive scheduler initialized\n");
    return 0;
}

/**
 * cognitive_sched_cleanup - Cleanup cognitive scheduler
 */
void cognitive_sched_cleanup(void)
{
    if (cognitive_task_cache) {
        kmem_cache_destroy(cognitive_task_cache);
        cognitive_task_cache = NULL;
    }
    
    pr_info("cognitive: Cognitive scheduler cleaned up\n");
}

/**
 * task_set_cognitive_attrs - Set cognitive attributes for a task
 * @task: Target task
 * @attrs: Cognitive attributes to set
 */
int task_set_cognitive_attrs(struct task_struct *task,
                           const struct cognitive_task *attrs)
{
    struct cognitive_task *cognitive_attrs;
    char atom_data[64];
    
    if (!task || !attrs) {
        return -EINVAL;
    }
    
    /* Allocate cognitive task structure */
    cognitive_attrs = kmem_cache_alloc(cognitive_task_cache, GFP_KERNEL);
    if (!cognitive_attrs) {
        return -ENOMEM;
    }
    
    /* Copy attributes */
    *cognitive_attrs = *attrs;
    
    /* Create AtomSpace representation of the task */
    snprintf(atom_data, sizeof(atom_data), "task_%d_%s", 
             task->pid, task->comm);
    cognitive_attrs->task_atom = atom_create(ATOM_TYPE_CONCEPT, 
                                           atom_data, strlen(atom_data));
    
    if (!cognitive_attrs->task_atom) {
        kmem_cache_free(cognitive_task_cache, cognitive_attrs);
        return -ENOMEM;
    }
    
    /* Set initial attention value based on priority */
    cognitive_attrs->attention_value = attrs->goal_priority * 10;
    
    /* Store in task structure (would need to extend task_struct) */
    task->cognitive_attrs = cognitive_attrs;
    
    return 0;
}

/**
 * task_get_cognitive_attrs - Get cognitive attributes for a task
 * @task: Target task
 */
struct cognitive_task *task_get_cognitive_attrs(struct task_struct *task)
{
    if (!task) {
        return NULL;
    }
    
    return task->cognitive_attrs;
}

/**
 * cognitive_sched_set_policy - Set cognitive scheduling policy
 * @policy: New scheduling policy
 */
int cognitive_sched_set_policy(enum cognitive_policy policy)
{
    if (policy >= COGNITIVE_POLICY_GOAL_DRIVEN + 1) {
        return -EINVAL;
    }
    
    cognitive_sched_config.policy = policy;
    
    /* Adjust configuration based on policy */
    switch (policy) {
    case COGNITIVE_POLICY_LEARNING:
        cognitive_sched_config.learning_boost = 50;
        cognitive_sched_config.reasoning_threshold = 30;
        break;
    case COGNITIVE_POLICY_REASONING:
        cognitive_sched_config.learning_boost = 10;
        cognitive_sched_config.reasoning_threshold = 70;
        break;
    case COGNITIVE_POLICY_REACTIVE:
        cognitive_sched_config.learning_boost = 5;
        cognitive_sched_config.reasoning_threshold = 20;
        break;
    case COGNITIVE_POLICY_GOAL_DRIVEN:
        cognitive_sched_config.enable_goal_scheduling = true;
        break;
    default:
        /* Balanced policy - use defaults */
        break;
    }
    
    pr_info("cognitive: Scheduling policy changed to %d\n", policy);
    return 0;
}

/**
 * cognitive_sched_get_policy - Get current cognitive scheduling policy
 */
enum cognitive_policy cognitive_sched_get_policy(void)
{
    return cognitive_sched_config.policy;
}

/**
 * cognitive_allocate_attention - Allocate attention to a task
 * @task: Target task
 * @amount: Amount of attention to allocate
 */
int cognitive_allocate_attention(struct task_struct *task, u32 amount)
{
    struct cognitive_rq *crq;
    struct cognitive_task *attrs;
    int cpu;
    
    if (!task) {
        return -EINVAL;
    }
    
    attrs = task_get_cognitive_attrs(task);
    if (!attrs) {
        return -ENOENT;
    }
    
    cpu = task_cpu(task);
    crq = &per_cpu(cognitive_rq, cpu);
    
    /* Check if enough attention is available */
    if (crq->available_attention < amount) {
        return -ENOSPC;
    }
    
    /* Allocate attention */
    crq->available_attention -= amount;
    attrs->attention_value += amount;
    
    return 0;
}

/**
 * cognitive_release_attention - Release attention from a task
 * @task: Target task
 * @amount: Amount of attention to release
 */
void cognitive_release_attention(struct task_struct *task, u32 amount)
{
    struct cognitive_rq *crq;
    struct cognitive_task *attrs;
    int cpu;
    
    if (!task) {
        return;
    }
    
    attrs = task_get_cognitive_attrs(task);
    if (!attrs) {
        return;
    }
    
    cpu = task_cpu(task);
    crq = &per_cpu(cognitive_rq, cpu);
    
    /* Release attention */
    if (attrs->attention_value >= amount) {
        attrs->attention_value -= amount;
        crq->available_attention += amount;
    }
}

/**
 * cognitive_get_available_attention - Get available attention on CPU
 * @cpu: CPU number
 */
u32 cognitive_get_available_attention(int cpu)
{
    struct cognitive_rq *crq = &per_cpu(cognitive_rq, cpu);
    return crq->available_attention;
}

/**
 * cognitive_set_goal - Set a goal for a task
 * @task: Target task
 * @goal: Goal atom
 * @priority: Goal priority
 */
int cognitive_set_goal(struct task_struct *task, struct atom *goal, u32 priority)
{
    struct cognitive_task *attrs;
    
    if (!task || !goal) {
        return -EINVAL;
    }
    
    attrs = task_get_cognitive_attrs(task);
    if (!attrs) {
        return -ENOENT;
    }
    
    /* Release previous goal if any */
    if (attrs->current_goal) {
        atom_put(attrs->current_goal);
    }
    
    /* Set new goal */
    attrs->current_goal = goal;
    attrs->goal_priority = priority;
    atomic_inc(&goal->ref_count);  /* Hold reference to goal */
    
    return 0;
}

/**
 * cognitive_get_current_goal - Get current goal for a task
 * @task: Target task
 */
struct atom *cognitive_get_current_goal(struct task_struct *task)
{
    struct cognitive_task *attrs;
    
    if (!task) {
        return NULL;
    }
    
    attrs = task_get_cognitive_attrs(task);
    if (!attrs) {
        return NULL;
    }
    
    return attrs->current_goal;
}

/**
 * cognitive_update_goal_progress - Update goal completion progress
 * @task: Target task
 * @progress: Progress percentage (0-100)
 */
int cognitive_update_goal_progress(struct task_struct *task, u32 progress)
{
    struct cognitive_task *attrs;
    struct cognitive_rq *crq;
    int cpu;
    
    if (!task || progress > 100) {
        return -EINVAL;
    }
    
    attrs = task_get_cognitive_attrs(task);
    if (!attrs || !attrs->current_goal) {
        return -ENOENT;
    }
    
    cpu = task_cpu(task);
    crq = &per_cpu(cognitive_rq, cpu);
    
    /* Update goal completion in the atom's truth value */
    attrs->current_goal->tv.strength = progress / 100.0f;
    attrs->current_goal->tv.confidence = 0.8f;  /* High confidence */
    
    /* Update CPU-level goal completion rate */
    crq->goal_completion_rate = (crq->goal_completion_rate + progress) / 2;
    
    return 0;
}

/**
 * cognitive_record_learning_event - Record a learning event
 * @task: Target task
 * @performance: Learning performance score (0-100)
 */
void cognitive_record_learning_event(struct task_struct *task, u32 performance)
{
    struct cognitive_task *attrs;
    struct cognitive_rq *crq;
    int cpu;
    
    if (!task) {
        return;
    }
    
    attrs = task_get_cognitive_attrs(task);
    if (!attrs) {
        return;
    }
    
    cpu = task_cpu(task);
    crq = &per_cpu(cognitive_rq, cpu);
    
    /* Update learning history */
    attrs->learning_history = (attrs->learning_history + performance) / 2;
    
    /* Update CPU-level learning cycles */
    crq->learning_cycles++;
    
    /* Boost attention for good learning performance */
    if (performance > 70) {
        cognitive_allocate_attention(task, cognitive_sched_config.learning_boost);
    }
}

/**
 * cognitive_update_adaptation_score - Update task adaptation score
 * @task: Target task
 * @score: Adaptation score (0-100)
 */
void cognitive_update_adaptation_score(struct task_struct *task, u32 score)
{
    struct cognitive_task *attrs;
    struct cognitive_rq *crq;
    int cpu;
    
    if (!task) {
        return;
    }
    
    attrs = task_get_cognitive_attrs(task);
    if (!attrs) {
        return;
    }
    
    cpu = task_cpu(task);
    crq = &per_cpu(cognitive_rq, cpu);
    
    /* Update adaptation score */
    attrs->adaptation_score = score;
    
    /* Update CPU-level average */
    crq->avg_adaptation_score = (crq->avg_adaptation_score + score) / 2;
}

EXPORT_SYMBOL(cognitive_sched_init);
EXPORT_SYMBOL(cognitive_sched_cleanup);
EXPORT_SYMBOL(task_set_cognitive_attrs);
EXPORT_SYMBOL(task_get_cognitive_attrs);
EXPORT_SYMBOL(cognitive_sched_set_policy);
EXPORT_SYMBOL(cognitive_sched_get_policy);
EXPORT_SYMBOL(cognitive_allocate_attention);
EXPORT_SYMBOL(cognitive_release_attention);
EXPORT_SYMBOL(cognitive_get_available_attention);
EXPORT_SYMBOL(cognitive_set_goal);
EXPORT_SYMBOL(cognitive_get_current_goal);
EXPORT_SYMBOL(cognitive_update_goal_progress);
EXPORT_SYMBOL(cognitive_record_learning_event);
EXPORT_SYMBOL(cognitive_update_adaptation_score);