// SPDX-License-Identifier: GPL-2.0
/**
 * Basic Ontogenesis Example
 * 
 * Demonstrates self-generation and self-optimization
 */

import {
  UniversalKernelGenerator,
  initializeOntogeneticKernel,
  selfGenerate,
  selfOptimize,
  visualizeTree
} from '../src/index';

console.log('=== CogLux Ontogenesis Example ===\n');

// 1. Generate a consciousness kernel
console.log('1. Generating consciousness kernel...');
const baseKernel = UniversalKernelGenerator.generateConsciousnessKernel(4);
console.log(`   Kernel ID: ${baseKernel.id}`);
console.log(`   Order: ${baseKernel.order}`);
console.log(`   Number of trees: ${baseKernel.trees.length}`);
console.log(`   Initial grip: ${baseKernel.grip.toFixed(4)}\n`);

// 2. Initialize as ontogenetic kernel
console.log('2. Initializing ontogenetic kernel...');
const ontoKernel = initializeOntogeneticKernel(baseKernel);
console.log(`   Genome ID: ${ontoKernel.genome.id}`);
console.log(`   Generation: ${ontoKernel.genome.generation}`);
console.log(`   Stage: ${ontoKernel.ontogeneticState.stage}`);
console.log(`   Fitness: ${ontoKernel.genome.fitness.toFixed(4)}\n`);

// 3. Self-generate offspring
console.log('3. Self-generating offspring...');
const offspring = selfGenerate(ontoKernel);
console.log(`   Offspring ID: ${offspring.genome.id}`);
console.log(`   Generation: ${offspring.genome.generation}`);
console.log(`   Parent: ${offspring.genome.lineage[0]}`);
console.log(`   Fitness: ${offspring.genome.fitness.toFixed(4)}`);
console.log(`   Grip: ${offspring.grip.toFixed(4)}\n`);

// 4. Self-optimize
console.log('4. Self-optimizing kernel...');
const optimized = selfOptimize(ontoKernel, 20);
console.log(`   Original fitness: ${ontoKernel.genome.fitness.toFixed(4)}`);
console.log(`   Optimized fitness: ${optimized.genome.fitness.toFixed(4)}`);
console.log(`   Maturity: ${optimized.ontogeneticState.maturity.toFixed(2)}`);
console.log(`   Stage: ${optimized.ontogeneticState.stage}`);
console.log(`   Reproductive capacity: ${optimized.ontogeneticState.reproductiveCapacity.toFixed(2)}\n`);

// 5. Visualize tree structures
console.log('5. Tree structures (first 3):');
for (let i = 0; i < Math.min(3, baseKernel.trees.length); i++) {
  console.log(visualizeTree(baseKernel.trees[i]));
}

// 6. Development history
console.log('6. Development history:');
optimized.ontogeneticState.developmentHistory.forEach(event => {
  console.log(`   Gen ${event.generation}: ${event.description}`);
  console.log(`      Stage: ${event.stage}, Fitness: ${event.fitness.toFixed(4)}`);
});

console.log('\n=== Example Complete ===');
