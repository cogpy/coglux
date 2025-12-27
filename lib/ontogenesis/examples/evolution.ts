// SPDX-License-Identifier: GPL-2.0
/**
 * Evolution Example
 * 
 * Demonstrates multi-generation evolution of kernel population
 */

import {
  UniversalKernelGenerator,
  runOntogenesis,
  OntogenesisConfig
} from '../src/index';

console.log('=== CogLux Ontogenesis Evolution Example ===\n');

// Configure evolution
const config: OntogenesisConfig = {
  evolution: {
    populationSize: 20,
    mutationRate: 0.15,
    crossoverRate: 0.8,
    elitismRate: 0.1,
    maxGenerations: 50,
    fitnessThreshold: 0.9,
    diversityPressure: 0.2
  },
  seedKernels: [
    UniversalKernelGenerator.generateConsciousnessKernel(4),
    UniversalKernelGenerator.generatePhysicsKernel(4)
  ]
};

console.log('Configuration:');
console.log(`  Population size: ${config.evolution.populationSize}`);
console.log(`  Mutation rate: ${config.evolution.mutationRate}`);
console.log(`  Crossover rate: ${config.evolution.crossoverRate}`);
console.log(`  Max generations: ${config.evolution.maxGenerations}`);
console.log(`  Fitness threshold: ${config.evolution.fitnessThreshold}`);
console.log(`  Seed kernels: ${config.seedKernels.length}\n`);

// Run evolution
console.log('Starting evolution...\n');
const result = runOntogenesis(config);

// Analyze results
console.log('\n=== Evolution Results ===\n');

console.log('Generation progress:');
result.generations.forEach((gen, i) => {
  if (i % 10 === 0 || i === result.generations.length - 1) {
    console.log(
      `  Gen ${gen.generation}: ` +
      `Best=${gen.bestFitness.toFixed(4)}, ` +
      `Avg=${gen.averageFitness.toFixed(4)}, ` +
      `Worst=${gen.worstFitness.toFixed(4)}, ` +
      `Diversity=${gen.diversity.toFixed(4)}`
    );
  }
});

console.log(`\nConvergence: Generation ${result.convergenceGeneration}`);
console.log(`Total generations: ${result.totalGenerations}`);

// Best kernel analysis
const bestKernel = result.generations[result.generations.length - 1].bestKernel;
console.log('\nBest kernel:');
console.log(`  Genome ID: ${bestKernel.genome.id}`);
console.log(`  Generation: ${bestKernel.genome.generation}`);
console.log(`  Fitness: ${bestKernel.genome.fitness.toFixed(4)}`);
console.log(`  Grip: ${bestKernel.grip.toFixed(4)}`);
console.log(`  Stage: ${bestKernel.ontogeneticState.stage}`);
console.log(`  Maturity: ${bestKernel.ontogeneticState.maturity.toFixed(2)}`);
console.log(`  Lineage depth: ${bestKernel.genome.lineage.length}`);

// Fitness improvement
const initialBest = result.generations[0].bestFitness;
const finalBest = result.generations[result.generations.length - 1].bestFitness;
const improvement = ((finalBest - initialBest) / initialBest * 100).toFixed(2);

console.log(`\nFitness improvement: ${improvement}%`);
console.log(`  Initial: ${initialBest.toFixed(4)}`);
console.log(`  Final: ${finalBest.toFixed(4)}`);

console.log('\n=== Evolution Complete ===');
