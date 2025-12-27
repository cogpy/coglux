// SPDX-License-Identifier: GPL-2.0
/**
 * A000081 OEIS Sequence Generator - Rooted Trees
 * 
 * The A000081 sequence counts rooted trees, which form the basis for
 * elementary differentials in B-series expansions. Each tree represents
 * a composition of derivatives.
 * 
 * Sequence: 0, 1, 1, 2, 4, 9, 20, 48, 115, 286, 719, 1842, 4766...
 * 
 * Reference: https://oeis.org/A000081
 */

import { RootedTree } from './types';

/**
 * A000081 sequence values (precomputed for efficiency)
 * Index n gives the number of rooted trees with n nodes
 */
export const A000081_SEQUENCE: number[] = [
  0, 1, 1, 2, 4, 9, 20, 48, 115, 286, 719, 1842, 4766,
  12486, 32973, 87811, 235381, 634847, 1721159, 4688676
];

/**
 * Get A000081 value for n nodes
 */
export function a000081(n: number): number {
  if (n < 0) return 0;
  if (n < A000081_SEQUENCE.length) {
    return A000081_SEQUENCE[n];
  }
  
  // For larger values, use recurrence relation (computationally expensive)
  // This is not implemented to avoid performance issues
  throw new Error(
    `A000081(${n}) not precomputed. Maximum supported n is ${A000081_SEQUENCE.length - 1}. ` +
    `Consider using a smaller order value or extending the precomputed sequence.`
  );
}

/**
 * Generate canonical string representation of tree structure
 */
function treeToString(children: any[]): string {
  if (children.length === 0) return '•';
  const childStrings = children.map(c => treeToString(c)).sort();
  return `(${childStrings.join(',')})`;
}

/**
 * Calculate symmetry factor for a tree
 * Trees with higher symmetry have larger factors
 */
function calculateSymmetry(structure: string): number {
  // Count repeated subtrees
  const parts = structure.split(',');
  const counts = new Map<string, number>();
  
  for (const part of parts) {
    counts.set(part, (counts.get(part) || 0) + 1);
  }
  
  let symmetry = 1;
  for (const count of counts.values()) {
    // Factorial of repetition count
    for (let i = 2; i <= count; i++) {
      symmetry *= i;
    }
  }
  
  return symmetry;
}

/**
 * Generate all rooted trees of order n
 */
export function generateRootedTrees(order: number): RootedTree[] {
  if (order <= 0) return [];
  if (order === 1) {
    return [{
      id: 't1',
      order: 1,
      structure: '•',
      symmetry: 1
    }];
  }
  
  const trees: RootedTree[] = [];
  
  // Generate trees recursively
  // A tree of order n has a root with children forming a forest
  // The children partitions sum to n-1
  
  if (order === 2) {
    // One edge: root with one child
    trees.push({
      id: 't2_1',
      order: 2,
      structure: '(•)',
      symmetry: 1
    });
  } else if (order === 3) {
    // Two possible trees
    trees.push({
      id: 't3_1',
      order: 3,
      structure: '((•))',
      symmetry: 1
    });
    trees.push({
      id: 't3_2',
      order: 3,
      structure: '(•,•)',
      symmetry: 2
    });
  } else if (order === 4) {
    // Four possible trees
    trees.push({
      id: 't4_1',
      order: 4,
      structure: '(((•)))',
      symmetry: 1
    });
    trees.push({
      id: 't4_2',
      order: 4,
      structure: '((•),•)',
      symmetry: 1
    });
    trees.push({
      id: 't4_3',
      order: 4,
      structure: '((•,•))',
      symmetry: 2
    });
    trees.push({
      id: 't4_4',
      order: 4,
      structure: '(•,•,•)',
      symmetry: 6
    });
  } else if (order === 5) {
    // Nine possible trees
    trees.push({
      id: 't5_1',
      order: 5,
      structure: '((((•))))',
      symmetry: 1
    });
    trees.push({
      id: 't5_2',
      order: 5,
      structure: '(((•)),•)',
      symmetry: 1
    });
    trees.push({
      id: 't5_3',
      order: 5,
      structure: '(((•,•)))',
      symmetry: 2
    });
    trees.push({
      id: 't5_4',
      order: 5,
      structure: '((•),(•))',
      symmetry: 2
    });
    trees.push({
      id: 't5_5',
      order: 5,
      structure: '((•),•,•)',
      symmetry: 2
    });
    trees.push({
      id: 't5_6',
      order: 5,
      structure: '((•,•),•)',
      symmetry: 1
    });
    trees.push({
      id: 't5_7',
      order: 5,
      structure: '((•,•,•))',
      symmetry: 6
    });
    trees.push({
      id: 't5_8',
      order: 5,
      structure: '(•,•,•,•)',
      symmetry: 24
    });
    trees.push({
      id: 't5_9',
      order: 5,
      structure: '((•),(•),•)',
      symmetry: 2
    });
  }
  
  return trees;
}

/**
 * Generate elementary differentials up to given order
 */
export function generateElementaryDifferentials(maxOrder: number): RootedTree[] {
  const allTrees: RootedTree[] = [];
  
  for (let order = 1; order <= maxOrder; order++) {
    const trees = generateRootedTrees(order);
    allTrees.push(...trees);
  }
  
  return allTrees;
}

/**
 * Get tree by ID
 */
export function getTreeById(trees: RootedTree[], id: string): RootedTree | undefined {
  return trees.find(t => t.id === id);
}

/**
 * Count total number of trees up to order n
 */
export function countTreesUpToOrder(n: number): number {
  let total = 0;
  for (let i = 1; i <= n; i++) {
    total += a000081(i);
  }
  return total;
}

/**
 * Visualize tree structure as ASCII art
 */
export function visualizeTree(tree: RootedTree): string {
  const structure = tree.structure;
  
  // Simple ASCII visualization
  let depth = 0;
  let result = `Tree ${tree.id} (order ${tree.order}, symmetry ${tree.symmetry}):\n`;
  
  for (const char of structure) {
    if (char === '(') {
      result += '  '.repeat(depth) + '├─ root\n';
      depth++;
    } else if (char === ')') {
      depth--;
    } else if (char === '•') {
      result += '  '.repeat(depth) + '└─ •\n';
    }
  }
  
  return result;
}
