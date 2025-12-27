// SPDX-License-Identifier: GPL-2.0
/**
 * CogLux Ontogenesis Library
 * 
 * Self-generating, evolving computational kernels using differential calculus.
 * 
 * This library implements living mathematical structures that can:
 * - Generate themselves through recursive composition
 * - Optimize themselves through iterative improvement
 * - Reproduce with other kernels through genetic operations
 * - Evolve across generations to maximize fitness
 * 
 * @example
 * ```typescript
 * import { 
 *   UniversalKernelGenerator, 
 *   initializeOntogeneticKernel,
 *   selfGenerate,
 *   runOntogenesis
 * } from 'coglux-ontogenesis';
 * 
 * // Generate a consciousness kernel
 * const kernel = UniversalKernelGenerator.generateConsciousnessKernel(4);
 * 
 * // Initialize for ontogenesis
 * const ontoKernel = initializeOntogeneticKernel(kernel);
 * 
 * // Self-generate offspring
 * const offspring = selfGenerate(ontoKernel);
 * 
 * // Evolve population
 * const config = {
 *   evolution: {
 *     populationSize: 20,
 *     mutationRate: 0.15,
 *     crossoverRate: 0.8,
 *     elitismRate: 0.1,
 *     maxGenerations: 50,
 *     fitnessThreshold: 0.9,
 *     diversityPressure: 0.2
 *   },
 *   seedKernels: [kernel]
 * };
 * 
 * const result = runOntogenesis(config);
 * ```
 */

// Type exports
export * from './types';

// A000081 sequence and rooted trees
export {
  A000081_SEQUENCE,
  a000081,
  generateRootedTrees,
  generateElementaryDifferentials,
  getTreeById,
  countTreesUpToOrder,
  visualizeTree
} from './a000081';

// Universal Kernel Generator
export {
  UniversalKernelGenerator,
  calculateGrip
} from './universal-kernel-generator';

// Ontogenetic operations
export {
  initializeOntogeneticKernel,
  selfGenerate,
  selfOptimize,
  selfReproduce,
  calculateFitness,
  calculateOverallFitness
} from './ontogenetic-operations';

// Evolution engine
export {
  runOntogenesis,
  runOntogenesisWithCallback
} from './evolution';
