// SPDX-License-Identifier: GPL-2.0
/*
 * Cognitive system calls for AGI-OS
 * 
 * Provides system call interface for userspace applications
 * to interact with the cognitive kernel
 */

#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/cognitive/atomspace.h>
#include <linux/cognitive/sched.h>

/* Cognitive system call numbers (would need to be added to syscall table) */
#define __NR_cognitive_base     450

#define __NR_cognitive_atom_create      (__NR_cognitive_base + 0)
#define __NR_cognitive_atom_query       (__NR_cognitive_base + 1)
#define __NR_cognitive_set_attrs        (__NR_cognitive_base + 2)
#define __NR_cognitive_set_goal         (__NR_cognitive_base + 3)
#define __NR_cognitive_get_attention    (__NR_cognitive_base + 4)

/**
 * sys_cognitive_atom_create - Create an atom in the kernel AtomSpace
 * @type: Atom type
 * @data: Atom data
 * @len: Data length
 * 
 * Returns: Atom UUID on success, negative error code on failure
 */
SYSCALL_DEFINE3(cognitive_atom_create, int, type, const char __user *, data, size_t, len)
{
    struct atom *atom;
    char *kernel_data;
    
    if (len > 256 || !data) {
        return -EINVAL;
    }
    
    /* Copy data from userspace */
    kernel_data = kmalloc(len + 1, GFP_KERNEL);
    if (!kernel_data) {
        return -ENOMEM;
    }
    
    if (copy_from_user(kernel_data, data, len)) {
        kfree(kernel_data);
        return -EFAULT;
    }
    kernel_data[len] = '\0';
    
    /* Create atom */
    atom = atom_create(type, kernel_data, len);
    kfree(kernel_data);
    
    if (!atom) {
        return -ENOMEM;
    }
    
    return atom->uuid;
}

/**
 * sys_cognitive_atom_query - Query atoms by pattern
 * @pattern: Query pattern
 * @result: Buffer for result UUID
 * 
 * Returns: 0 on success, negative error code on failure
 */
SYSCALL_DEFINE2(cognitive_atom_query, const char __user *, pattern, u64 __user *, result)
{
    struct atom *atom;
    char *kernel_pattern;
    size_t len;
    
    if (!pattern || !result) {
        return -EINVAL;
    }
    
    /* Get pattern length */
    len = strnlen_user(pattern, 256);
    if (len > 256) {
        return -EINVAL;
    }
    
    /* Copy pattern from userspace */
    kernel_pattern = kmalloc(len, GFP_KERNEL);
    if (!kernel_pattern) {
        return -ENOMEM;
    }
    
    if (copy_from_user(kernel_pattern, pattern, len)) {
        kfree(kernel_pattern);
        return -EFAULT;
    }
    
    /* Query atom */
    atom = atomspace_query(kernel_pattern);
    kfree(kernel_pattern);
    
    if (!atom) {
        return -ENOENT;
    }
    
    /* Return UUID to userspace */
    if (put_user(atom->uuid, result)) {
        atom_put(atom);
        return -EFAULT;
    }
    
    atom_put(atom);
    return 0;
}

/**
 * sys_cognitive_set_attrs - Set cognitive attributes for current task
 * @attrs: Cognitive attributes structure
 * 
 * Returns: 0 on success, negative error code on failure
 */
SYSCALL_DEFINE1(cognitive_set_attrs, const struct cognitive_task __user *, attrs)
{
    struct cognitive_task kernel_attrs;
    
    if (!attrs) {
        return -EINVAL;
    }
    
    /* Copy attributes from userspace */
    if (copy_from_user(&kernel_attrs, attrs, sizeof(kernel_attrs))) {
        return -EFAULT;
    }
    
    /* Set attributes for current task */
    return task_set_cognitive_attrs(current, &kernel_attrs);
}

/**
 * sys_cognitive_set_goal - Set a goal for current task
 * @goal_uuid: UUID of goal atom
 * @priority: Goal priority
 * 
 * Returns: 0 on success, negative error code on failure
 */
SYSCALL_DEFINE2(cognitive_set_goal, u64, goal_uuid, u32, priority)
{
    struct atom *goal;
    
    /* Get goal atom */
    goal = atom_get(goal_uuid);
    if (!goal) {
        return -ENOENT;
    }
    
    /* Set goal for current task */
    return cognitive_set_goal(current, goal, priority);
}

/**
 * sys_cognitive_get_attention - Get available attention on current CPU
 * 
 * Returns: Available attention units
 */
SYSCALL_DEFINE0(cognitive_get_attention)
{
    return cognitive_get_available_attention(smp_processor_id());
}

/* Export syscalls for registration */
EXPORT_SYMBOL(sys_cognitive_atom_create);
EXPORT_SYMBOL(sys_cognitive_atom_query);
EXPORT_SYMBOL(sys_cognitive_set_attrs);
EXPORT_SYMBOL(sys_cognitive_set_goal);
EXPORT_SYMBOL(sys_cognitive_get_attention);