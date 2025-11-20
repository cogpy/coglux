/* SPDX-License-Identifier: GPL-2.0 */
/*
 * OpenCog AtomSpace implementation for Linux kernel
 * 
 * This provides the core knowledge representation system for the
 * AGI-OS cognitive kernel, implementing OpenCog's AtomSpace in
 * kernel space for high-performance cognitive computing.
 */

#ifndef _LINUX_COGNITIVE_ATOMSPACE_H
#define _LINUX_COGNITIVE_ATOMSPACE_H

#include <linux/types.h>
#include <linux/atomic.h>
#include <linux/rbtree.h>
#include <linux/list.h>
#include <linux/spinlock.h>

/* Atom types for cognitive processing */
enum atom_type {
    ATOM_TYPE_NODE = 0,
    ATOM_TYPE_LINK,
    ATOM_TYPE_CONCEPT,
    ATOM_TYPE_PREDICATE,
    ATOM_TYPE_EVALUATION,
    ATOM_TYPE_IMPLICATION,
    ATOM_TYPE_EXECUTION,
    ATOM_TYPE_MAX
};

/* AtomSpace configuration constants */
#define ATOM_MAX_DATA_SIZE    256    /* Maximum size of atom data */
#define ATOM_MAX_UUID         (1ULL << 60)  /* Maximum atom UUID */
#define ATOM_CACHE_SIZE       (sizeof(struct atom) + ATOM_MAX_DATA_SIZE)
/* Truth value representation for uncertain reasoning */
struct truth_value {
    float strength;   /* Probability/confidence [0.0, 1.0] */
    float confidence; /* Reliability of strength [0.0, 1.0] */
};

/* Core atom structure for knowledge representation */
struct atom {
    u64 uuid;                   /* Unique identifier */
    enum atom_type type;        /* Type of atom */
    struct truth_value tv;      /* Truth value for uncertain reasoning */
    atomic_t ref_count;         /* Reference counting for memory management */
    
    /* Tree structure for efficient lookups */
    struct rb_node node;
    
    /* Incoming and outgoing relationships */
    struct list_head incoming;  /* Links pointing to this atom */
    struct list_head outgoing;  /* Links this atom points to */
    
    /* Atom data */
    size_t data_len;
    char data[];               /* Variable length atom data */
};

/* AtomSpace structure - the cognitive knowledge base */
struct atomspace {
    struct rb_root atoms;      /* Red-black tree of all atoms */
    spinlock_t lock;          /* Synchronization for concurrent access */
    atomic64_t next_uuid;     /* UUID generator */
    atomic_t atom_count;      /* Total number of atoms */
    
    /* Memory management */
    struct kmem_cache *atom_cache;
    
    /* Pattern matching support */
    struct rb_root patterns;  /* Indexed patterns for fast matching */
};

/* Global kernel AtomSpace instance */
extern struct atomspace kernel_atomspace;

/* AtomSpace operations */
int atomspace_init(void);
void atomspace_cleanup(void);

struct atom *atom_create(enum atom_type type, const char *data, size_t len);
struct atom *atom_get(u64 uuid);
void atom_put(struct atom *atom);
int atom_link(struct atom *from, struct atom *to);

/* Pattern matching and querying */
struct atom *atomspace_query(const char *pattern);
int atomspace_pattern_match(const char *pattern, 
                           int (*callback)(struct atom *, void *),
                           void *data);

/* Truth value operations */
static inline void truth_value_init(struct truth_value *tv, 
                                   float strength, float confidence)
{
    tv->strength = strength;
    tv->confidence = confidence;
}

static inline float truth_value_mean(const struct truth_value *tv)
{
    return tv->strength;
}

#endif /* _LINUX_COGNITIVE_ATOMSPACE_H */