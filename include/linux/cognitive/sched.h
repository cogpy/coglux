/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Cognitive scheduler for AGI-OS
 * 
 * This implements a cognitive-aware scheduler that uses OpenCog's
 * reasoning capabilities to make intelligent scheduling decisions
 * based on task cognitive load, learning patterns, and system goals.
 */

#ifndef _LINUX_COGNITIVE_SCHED_H
#define _LINUX_COGNITIVE_SCHED_H

#include <linux/sched.h>
#include <linux/cognitive/atomspace.h>

/* Cognitive task attributes */
struct cognitive_task {
    struct atom *task_atom;        /* AtomSpace representation of task */
    
    /* Cognitive workload characteristics */
    u32 reasoning_load;            /* Reasoning complexity [0-100] */
    u32 memory_access_pattern;     /* Memory access cognitive pattern */
    u32 learning_rate;             /* How fast task learns/adapts */
    
    /* Goal-oriented scheduling */
    struct atom *current_goal;     /* Current cognitive goal */
    u32 goal_priority;            /* Goal priority level */
    
    /* Attention and focus management */
    u32 attention_value;          /* Current attention allocation */
    u64 focus_duration;           /* How long task can maintain focus */
    
    /* Learning and adaptation */
    u32 adaptation_score;         /* How well task adapts to changes */
    u64 learning_history;         /* Historical learning performance */
};

/* Cognitive scheduling policy */
enum cognitive_policy {
    COGNITIVE_POLICY_BALANCED,    /* Balance all cognitive factors */
    COGNITIVE_POLICY_LEARNING,    /* Prioritize learning tasks */
    COGNITIVE_POLICY_REASONING,   /* Prioritize reasoning-heavy tasks */
    COGNITIVE_POLICY_REACTIVE,    /* Prioritize reactive/real-time tasks */
    COGNITIVE_POLICY_GOAL_DRIVEN  /* Prioritize based on system goals */
};

/* Cognitive scheduler configuration */
struct cognitive_config {
    enum cognitive_policy policy;
    u32 attention_budget;         /* Total attention units available */
    u32 learning_boost;          /* Boost factor for learning tasks */
    u32 reasoning_threshold;     /* Threshold for reasoning task priority */
    bool enable_goal_scheduling; /* Enable goal-oriented scheduling */
};

/* Per-CPU cognitive scheduler state */
struct cognitive_rq {
    struct rb_root cognitive_tasks;  /* Red-black tree of cognitive tasks */
    u32 total_attention;            /* Total attention allocated */
    u32 available_attention;        /* Available attention units */
    
    /* Goal tracking */
    struct atom *active_goals;      /* Currently active goals */
    u32 goal_completion_rate;       /* Rate of goal completion */
    
    /* Learning optimization */
    u64 learning_cycles;           /* Number of learning cycles completed */
    u32 avg_adaptation_score;      /* Average adaptation performance */
};

/* Global cognitive scheduler interface */
extern struct cognitive_config cognitive_sched_config;

/* Cognitive scheduler operations */
int cognitive_sched_init(void);
void cognitive_sched_cleanup(void);

/* Task cognitive attribute management */
int task_set_cognitive_attrs(struct task_struct *task, 
                           const struct cognitive_task *attrs);
struct cognitive_task *task_get_cognitive_attrs(struct task_struct *task);

/* Cognitive scheduling policy */
int cognitive_sched_set_policy(enum cognitive_policy policy);
enum cognitive_policy cognitive_sched_get_policy(void);

/* Attention management */
int cognitive_allocate_attention(struct task_struct *task, u32 amount);
void cognitive_release_attention(struct task_struct *task, u32 amount);
u32 cognitive_get_available_attention(int cpu);

/* Goal-oriented scheduling */
int cognitive_set_goal(struct task_struct *task, struct atom *goal, u32 priority);
struct atom *cognitive_get_current_goal(struct task_struct *task);
int cognitive_update_goal_progress(struct task_struct *task, u32 progress);

/* Learning and adaptation tracking */
void cognitive_record_learning_event(struct task_struct *task, u32 performance);
void cognitive_update_adaptation_score(struct task_struct *task, u32 score);

/* Scheduler integration helpers */
static inline bool is_cognitive_task(struct task_struct *task)
{
    return task->cognitive_attrs != NULL;
}

static inline u32 get_cognitive_priority(struct task_struct *task)
{
    struct cognitive_task *attrs = task_get_cognitive_attrs(task);
    return attrs ? attrs->goal_priority : 0;
}

#endif /* _LINUX_COGNITIVE_SCHED_H */