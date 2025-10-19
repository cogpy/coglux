// SPDX-License-Identifier: GPL-2.0
/*
 * Pattern matching engine implementation for cognitive kernel
 */

#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/rbtree.h>
#include <linux/cognitive/pattern.h>
#include <linux/cognitive/atomspace.h>

/* Pattern index for fast lookups */
static struct rb_root pattern_index = RB_ROOT;
static DEFINE_SPINLOCK(pattern_index_lock);

/* Memory cache for pattern nodes */
static struct kmem_cache *pattern_node_cache;

/**
 * pattern_engine_init - Initialize pattern matching engine
 */
int __init pattern_engine_init(void)
{
    /* Create memory cache for pattern nodes */
    pattern_node_cache = kmem_cache_create("cognitive_patterns",
                                         sizeof(struct pattern_node),
                                         0, SLAB_HWCACHE_ALIGN, NULL);
    if (!pattern_node_cache) {
        pr_err("cognitive: Failed to create pattern node cache\n");
        return -ENOMEM;
    }
    
    pr_info("cognitive: Pattern matching engine initialized\n");
    return 0;
}

/**
 * pattern_engine_cleanup - Cleanup pattern matching engine
 */
void pattern_engine_cleanup(void)
{
    if (pattern_node_cache) {
        kmem_cache_destroy(pattern_node_cache);
        pattern_node_cache = NULL;
    }
    
    pr_info("cognitive: Pattern matching engine cleaned up\n");
}

/**
 * pattern_parse - Parse a pattern string into pattern nodes
 * @pattern_str: Pattern string to parse
 * 
 * Returns: Root pattern node or NULL on failure
 */
struct pattern_node *pattern_parse(const char *pattern_str)
{
    struct pattern_node *node;
    
    if (!pattern_str) {
        return NULL;
    }
    
    /* Allocate pattern node */
    node = kmem_cache_alloc(pattern_node_cache, GFP_KERNEL);
    if (!node) {
        return NULL;
    }
    
    /* Initialize node */
    INIT_LIST_HEAD(&node->children);
    INIT_LIST_HEAD(&node->siblings);
    node->variable_name = NULL;
    
    /* Simple pattern parsing - would be more sophisticated in full implementation */
    if (pattern_str[0] == '?') {
        /* Variable pattern */
        node->type = PATTERN_VARIABLE;
        node->variable_name = kstrdup(pattern_str + 1, GFP_KERNEL);
    } else if (strcmp(pattern_str, "*") == 0) {
        /* Wildcard pattern */
        node->type = PATTERN_WILDCARD;
    } else {
        /* Exact match pattern */
        node->type = PATTERN_EXACT;
    }
    
    return node;
}

/**
 * pattern_free - Free a pattern node tree
 * @pattern: Root pattern node to free
 */
void pattern_free(struct pattern_node *pattern)
{
    struct pattern_node *child, *tmp;
    
    if (!pattern) {
        return;
    }
    
    /* Free all children recursively */
    list_for_each_entry_safe(child, tmp, &pattern->children, siblings) {
        pattern_free(child);
    }
    
    /* Free variable name if any */
    if (pattern->variable_name) {
        kfree(pattern->variable_name);
    }
    
    /* Free the node */
    kmem_cache_free(pattern_node_cache, pattern);
}

/**
 * pattern_match_atom - Check if an atom matches a pattern node
 * @atom: Atom to test
 * @pattern: Pattern node to match against
 * 
 * Returns: true if match, false otherwise
 */
static bool pattern_match_atom(struct atom *atom, struct pattern_node *pattern)
{
    if (!atom || !pattern) {
        return false;
    }
    
    switch (pattern->type) {
    case PATTERN_EXACT:
        /* For exact match, would need to compare atom data */
        return true;  /* Simplified for now */
        
    case PATTERN_VARIABLE:
        /* Variables match any atom */
        return true;
        
    case PATTERN_WILDCARD:
        /* Wildcards match any atom */
        return true;
        
    case PATTERN_TYPE:
        /* Type constraint */
        return atom->type == pattern->atom_type_constraint;
        
    default:
        return false;
    }
}

/**
 * pattern_match - Perform pattern matching in AtomSpace
 * @pattern: Pattern to match
 * @context: Pattern matching context
 * 
 * Returns: Number of matches found
 */
int pattern_match(struct pattern_node *pattern, struct pattern_context *context)
{
    struct rb_node *node;
    struct atom *atom;
    struct query_result *result;
    int matches = 0;
    
    if (!pattern || !context) {
        return -EINVAL;
    }
    
    /* Initialize results */
    INIT_LIST_HEAD(&context->results);
    
    /* Iterate through all atoms in AtomSpace */
    spin_lock(&kernel_atomspace.lock);
    
    for (node = rb_first(&kernel_atomspace.atoms); 
         node && matches < context->max_results; 
         node = rb_next(node)) {
         
        atom = rb_entry(node, struct atom, node);
        
        /* Test if atom matches pattern */
        if (pattern_match_atom(atom, pattern)) {
            /* Create result entry */
            result = kmalloc(sizeof(*result), GFP_ATOMIC);
            if (!result) {
                break;
            }
            
            /* For now, just store the atom reference */
            result->bindings = kmalloc(sizeof(struct atom *), GFP_ATOMIC);
            if (!result->bindings) {
                kfree(result);
                break;
            }
            
            result->bindings[0] = atom;
            result->binding_count = 1;
            truth_value_init(&result->confidence, 1.0f, 0.8f);
            
            /* Add to results */
            list_add_tail(&result->list, &context->results);
            matches++;
            
            /* Hold reference to atom */
            atomic_inc(&atom->ref_count);
        }
    }
    
    spin_unlock(&kernel_atomspace.lock);
    
    return matches;
}

/**
 * pattern_results_free - Free pattern matching results
 * @context: Pattern context containing results
 */
void pattern_results_free(struct pattern_context *context)
{
    struct query_result *result, *tmp;
    
    if (!context) {
        return;
    }
    
    list_for_each_entry_safe(result, tmp, &context->results, list) {
        /* Release atom references */
        if (result->bindings) {
            int i;
            for (i = 0; i < result->binding_count; i++) {
                if (result->bindings[i]) {
                    atom_put(result->bindings[i]);
                }
            }
            kfree(result->bindings);
        }
        
        list_del(&result->list);
        kfree(result);
    }
}

/**
 * pattern_match_type - Match atoms by type
 * @type: Atom type to match
 * @callback: Callback function for each match
 * @data: User data for callback
 * 
 * Returns: Number of matches found
 */
int pattern_match_type(enum atom_type type,
                      int (*callback)(struct atom *, void *),
                      void *data)
{
    struct rb_node *node;
    struct atom *atom;
    int matches = 0;
    
    if (!callback) {
        return -EINVAL;
    }
    
    spin_lock(&kernel_atomspace.lock);
    
    for (node = rb_first(&kernel_atomspace.atoms); node; node = rb_next(node)) {
        atom = rb_entry(node, struct atom, node);
        
        if (atom->type == type) {
            atomic_inc(&atom->ref_count);
            spin_unlock(&kernel_atomspace.lock);
            
            /* Call callback */
            if (callback(atom, data) != 0) {
                atom_put(atom);
                return matches;  /* Stop on callback error */
            }
            
            atom_put(atom);
            matches++;
            spin_lock(&kernel_atomspace.lock);
        }
    }
    
    spin_unlock(&kernel_atomspace.lock);
    return matches;
}

/**
 * pattern_match_predicate - Match atoms by predicate
 * @predicate: Predicate name to match
 * @callback: Callback function for each match
 * @data: User data for callback
 * 
 * Returns: Number of matches found
 */
int pattern_match_predicate(const char *predicate,
                           int (*callback)(struct atom *, void *),
                           void *data)
{
    struct rb_node *node;
    struct atom *atom;
    int matches = 0;
    
    if (!predicate || !callback) {
        return -EINVAL;
    }
    
    spin_lock(&kernel_atomspace.lock);
    
    for (node = rb_first(&kernel_atomspace.atoms); node; node = rb_next(node)) {
        atom = rb_entry(node, struct atom, node);
        
        /* Simple string matching for predicates */
        if (atom->data_len > 0 && 
            strstr(atom->data, predicate) != NULL) {
            
            atomic_inc(&atom->ref_count);
            spin_unlock(&kernel_atomspace.lock);
            
            /* Call callback */
            if (callback(atom, data) != 0) {
                atom_put(atom);
                return matches;  /* Stop on callback error */
            }
            
            atom_put(atom);
            matches++;
            spin_lock(&kernel_atomspace.lock);
        }
    }
    
    spin_unlock(&kernel_atomspace.lock);
    return matches;
}

/**
 * pattern_index_atom - Add atom to pattern index
 * @atom: Atom to index
 */
int pattern_index_atom(struct atom *atom)
{
    /* Pattern indexing would build specialized indices
     * for different atom types and patterns for fast lookup */
    return 0;  /* Simplified for now */
}

/**
 * pattern_unindex_atom - Remove atom from pattern index  
 * @atom: Atom to remove from index
 */
void pattern_unindex_atom(struct atom *atom)
{
    /* Remove from pattern indices */
    /* Simplified for now */
}

EXPORT_SYMBOL(pattern_engine_init);
EXPORT_SYMBOL(pattern_engine_cleanup);
EXPORT_SYMBOL(pattern_parse);
EXPORT_SYMBOL(pattern_free);
EXPORT_SYMBOL(pattern_match);
EXPORT_SYMBOL(pattern_results_free);
EXPORT_SYMBOL(pattern_match_type);
EXPORT_SYMBOL(pattern_match_predicate);
EXPORT_SYMBOL(pattern_index_atom);
EXPORT_SYMBOL(pattern_unindex_atom);