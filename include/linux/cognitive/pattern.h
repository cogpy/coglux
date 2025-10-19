/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Pattern matching engine for cognitive kernel
 * 
 * Provides high-performance pattern matching and unification
 * for the OpenCog AtomSpace in kernel space
 */

#ifndef _LINUX_COGNITIVE_PATTERN_H
#define _LINUX_COGNITIVE_PATTERN_H

#include <linux/types.h>
#include <linux/cognitive/atomspace.h>

/* Pattern matching types */
enum pattern_type {
    PATTERN_EXACT,          /* Exact match */
    PATTERN_VARIABLE,       /* Variable binding */
    PATTERN_WILDCARD,       /* Wildcard match */
    PATTERN_TYPE,           /* Type constraint */
    PATTERN_PREDICATE       /* Predicate constraint */
};

/* Pattern node for query representation */
struct pattern_node {
    enum pattern_type type;
    enum atom_type atom_type_constraint;
    char *variable_name;
    struct list_head children;
    struct list_head siblings;
};

/* Query result set */
struct query_result {
    struct atom **bindings;    /* Variable bindings */
    size_t binding_count;
    struct truth_value confidence;
    struct list_head list;
};

/* Pattern matching context */
struct pattern_context {
    struct pattern_node *pattern;
    struct list_head results;
    size_t max_results;
    u32 timeout_ms;
};

/* Pattern matching operations */
int pattern_engine_init(void);
void pattern_engine_cleanup(void);

struct pattern_node *pattern_parse(const char *pattern_str);
void pattern_free(struct pattern_node *pattern);

int pattern_match(struct pattern_node *pattern, 
                 struct pattern_context *context);
void pattern_results_free(struct pattern_context *context);

/* Specialized pattern matching for common cases */
int pattern_match_type(enum atom_type type, 
                      int (*callback)(struct atom *, void *),
                      void *data);

int pattern_match_predicate(const char *predicate,
                          int (*callback)(struct atom *, void *),
                          void *data);

/* Pattern indexing for performance */
int pattern_index_atom(struct atom *atom);
void pattern_unindex_atom(struct atom *atom);

#endif /* _LINUX_COGNITIVE_PATTERN_H */