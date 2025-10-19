// SPDX-License-Identifier: GPL-2.0
/*
 * OpenCog AtomSpace implementation for Linux kernel
 * 
 * Core knowledge representation system for AGI-OS cognitive kernel
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/rbtree.h>
#include <linux/atomic.h>
#include <linux/string.h>
#include <linux/cognitive/atomspace.h>

/* Global kernel AtomSpace instance */
struct atomspace kernel_atomspace;

/* Atom memory cache for efficient allocation */
static struct kmem_cache *atom_cache;

/**
 * atomspace_init - Initialize the kernel AtomSpace
 * 
 * Sets up the global AtomSpace for cognitive operations
 */
int __init atomspace_init(void)
{
    /* Initialize the AtomSpace structure */
    kernel_atomspace.atoms = RB_ROOT;
    spin_lock_init(&kernel_atomspace.lock);
    atomic64_set(&kernel_atomspace.next_uuid, 1);
    atomic_set(&kernel_atomspace.atom_count, 0);
    kernel_atomspace.patterns = RB_ROOT;
    
    /* Create memory cache for atoms */
    atom_cache = kmem_cache_create("cognitive_atoms",
                                   sizeof(struct atom) + 256,  /* Base size + data */
                                   0, SLAB_HWCACHE_ALIGN, NULL);
    if (!atom_cache) {
        pr_err("cognitive: Failed to create atom cache\n");
        return -ENOMEM;
    }
    
    kernel_atomspace.atom_cache = atom_cache;
    
    pr_info("cognitive: AtomSpace initialized\n");
    return 0;
}

/**
 * atomspace_cleanup - Cleanup the kernel AtomSpace
 */
void atomspace_cleanup(void)
{
    struct rb_node *node;
    struct atom *atom;
    
    /* Clean up all atoms */
    spin_lock(&kernel_atomspace.lock);
    while ((node = rb_first(&kernel_atomspace.atoms))) {
        atom = rb_entry(node, struct atom, node);
        rb_erase(node, &kernel_atomspace.atoms);
        kmem_cache_free(atom_cache, atom);
    }
    spin_unlock(&kernel_atomspace.lock);
    
    /* Destroy the memory cache */
    if (atom_cache) {
        kmem_cache_destroy(atom_cache);
        atom_cache = NULL;
    }
    
    pr_info("cognitive: AtomSpace cleaned up\n");
}

/**
 * atom_create - Create a new atom in the AtomSpace
 * @type: Type of atom to create
 * @data: Atom data content
 * @len: Length of data
 * 
 * Returns: Pointer to new atom or NULL on failure
 */
struct atom *atom_create(enum atom_type type, const char *data, size_t len)
{
    struct atom *atom;
    struct rb_node **link, *parent = NULL;
    u64 uuid;
    
    if (len > 256) {  /* Limit atom data size */
        return NULL;
    }
    
    /* Allocate atom from cache */
    atom = kmem_cache_alloc(atom_cache, GFP_KERNEL);
    if (!atom) {
        return NULL;
    }
    
    /* Initialize atom */
    uuid = atomic64_inc_return(&kernel_atomspace.next_uuid);
    atom->uuid = uuid;
    atom->type = type;
    truth_value_init(&atom->tv, 1.0f, 0.0f);  /* Default truth value */
    atomic_set(&atom->ref_count, 1);
    INIT_LIST_HEAD(&atom->incoming);
    INIT_LIST_HEAD(&atom->outgoing);
    
    /* Copy atom data */
    atom->data_len = len;
    if (data && len > 0) {
        memcpy(atom->data, data, len);
    }
    
    /* Insert into AtomSpace tree */
    spin_lock(&kernel_atomspace.lock);
    
    link = &kernel_atomspace.atoms.rb_node;
    while (*link) {
        struct atom *this_atom;
        
        parent = *link;
        this_atom = rb_entry(parent, struct atom, node);
        
        if (uuid < this_atom->uuid) {
            link = &(*link)->rb_left;
        } else if (uuid > this_atom->uuid) {
            link = &(*link)->rb_right;
        } else {
            /* UUID collision (should not happen) */
            spin_unlock(&kernel_atomspace.lock);
            kmem_cache_free(atom_cache, atom);
            return NULL;
        }
    }
    
    rb_link_node(&atom->node, parent, link);
    rb_insert_color(&atom->node, &kernel_atomspace.atoms);
    atomic_inc(&kernel_atomspace.atom_count);
    
    spin_unlock(&kernel_atomspace.lock);
    
    return atom;
}

/**
 * atom_get - Retrieve an atom by UUID
 * @uuid: Unique identifier of the atom
 * 
 * Returns: Pointer to atom or NULL if not found
 */
struct atom *atom_get(u64 uuid)
{
    struct rb_node *node;
    struct atom *atom;
    
    spin_lock(&kernel_atomspace.lock);
    
    node = kernel_atomspace.atoms.rb_node;
    while (node) {
        atom = rb_entry(node, struct atom, node);
        
        if (uuid < atom->uuid) {
            node = node->rb_left;
        } else if (uuid > atom->uuid) {
            node = node->rb_right;
        } else {
            /* Found the atom, increment reference count */
            atomic_inc(&atom->ref_count);
            spin_unlock(&kernel_atomspace.lock);
            return atom;
        }
    }
    
    spin_unlock(&kernel_atomspace.lock);
    return NULL;
}

/**
 * atom_put - Release reference to an atom
 * @atom: Atom to release
 */
void atom_put(struct atom *atom)
{
    if (!atom) {
        return;
    }
    
    if (atomic_dec_and_test(&atom->ref_count)) {
        /* Last reference, remove from AtomSpace */
        spin_lock(&kernel_atomspace.lock);
        rb_erase(&atom->node, &kernel_atomspace.atoms);
        atomic_dec(&kernel_atomspace.atom_count);
        spin_unlock(&kernel_atomspace.lock);
        
        /* Free the atom */
        kmem_cache_free(atom_cache, atom);
    }
}

/**
 * atom_link - Create a relationship between two atoms
 * @from: Source atom
 * @to: Target atom
 * 
 * Returns: 0 on success, negative error code on failure
 */
int atom_link(struct atom *from, struct atom *to)
{
    /* This is a simplified version - in a full implementation,
     * we would create proper link atoms and manage the relationships
     * more sophisticatedly */
    
    if (!from || !to) {
        return -EINVAL;
    }
    
    /* For now, just increment reference counts to maintain relationships */
    atomic_inc(&from->ref_count);
    atomic_inc(&to->ref_count);
    
    return 0;
}

/**
 * atomspace_query - Simple pattern matching query
 * @pattern: Query pattern string
 * 
 * Returns: First matching atom or NULL
 */
struct atom *atomspace_query(const char *pattern)
{
    struct rb_node *node;
    struct atom *atom;
    
    if (!pattern) {
        return NULL;
    }
    
    /* Simple linear search - in a full implementation this would
     * use indexed pattern matching for efficiency */
    spin_lock(&kernel_atomspace.lock);
    
    for (node = rb_first(&kernel_atomspace.atoms); node; node = rb_next(node)) {
        atom = rb_entry(node, struct atom, node);
        
        /* Simple string matching for now */
        if (atom->data_len > 0 && 
            strstr(atom->data, pattern) != NULL) {
            atomic_inc(&atom->ref_count);
            spin_unlock(&kernel_atomspace.lock);
            return atom;
        }
    }
    
    spin_unlock(&kernel_atomspace.lock);
    return NULL;
}

/**
 * atomspace_pattern_match - Pattern matching with callback
 * @pattern: Pattern to match
 * @callback: Function to call for each match
 * @data: User data to pass to callback
 * 
 * Returns: Number of matches found
 */
int atomspace_pattern_match(const char *pattern,
                           int (*callback)(struct atom *, void *),
                           void *data)
{
    struct rb_node *node;
    struct atom *atom;
    int matches = 0;
    
    if (!pattern || !callback) {
        return -EINVAL;
    }
    
    spin_lock(&kernel_atomspace.lock);
    
    for (node = rb_first(&kernel_atomspace.atoms); node; node = rb_next(node)) {
        atom = rb_entry(node, struct atom, node);
        
        /* Simple pattern matching */
        if (atom->data_len > 0 && 
            strstr(atom->data, pattern) != NULL) {
            
            atomic_inc(&atom->ref_count);
            spin_unlock(&kernel_atomspace.lock);
            
            /* Call the callback */
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

EXPORT_SYMBOL(atomspace_init);
EXPORT_SYMBOL(atomspace_cleanup);
EXPORT_SYMBOL(atom_create);
EXPORT_SYMBOL(atom_get);
EXPORT_SYMBOL(atom_put);
EXPORT_SYMBOL(atom_link);
EXPORT_SYMBOL(atomspace_query);
EXPORT_SYMBOL(atomspace_pattern_match);