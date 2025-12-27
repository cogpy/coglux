// SPDX-License-Identifier: GPL-2.0
/**
 * Ontogenetic Operations
 * 
 * Core operations for self-generating kernels:
 * - Self-generation: Recursive self-composition using chain rule
 * - Self-optimization: Iterative grip improvement
 * - Self-reproduction: Genetic crossover and mutation
 */

import {
  GeneratedKernel,
  OntogeneticKernel,
  KernelGenome,
  KernelGene,
  OntogeneticState,
  DevelopmentStage,
  DevelopmentEvent,
  ReproductionMethod,
  FitnessMetrics
} from './types';
import { calculateGrip } from './universal-kernel-generator';

/**
 * Generate unique genome ID
 * Note: Uses timestamp + random for simplicity. For production use,
 * consider crypto.randomUUID() or a proper UUID library.
 */
function generateGenomeId(): string {
  return `genome_${Date.now()}_${Math.random().toString(36).substring(2, 11)}`;
}

/**
 * Convert kernel to genes
 */
function kernelToGenes(kernel: GeneratedKernel): KernelGene[] {
  return kernel.trees.map((tree, i) => ({
    id: `gene_${tree.id}`,
    differential: {
      tree,
      coefficient: kernel.coefficients[i],
      weight: 1.0 / (tree.order * tree.symmetry)
    },
    expression: [kernel.coefficients[i]],
    dominance: Math.abs(kernel.coefficients[i]),
    mutable: true
  }));
}

/**
 * Calculate comprehensive fitness metrics
 */
export function calculateFitness(kernel: OntogeneticKernel): FitnessMetrics {
  const grip = kernel.grip;
  
  // Stability: coefficient variance (lower is more stable)
  const coeffs = kernel.coefficients;
  const mean = coeffs.reduce((a, b) => a + b, 0) / coeffs.length;
  const variance = coeffs.reduce((sum, c) => sum + Math.pow(c - mean, 2), 0) / coeffs.length;
  const stability = 1.0 / (1.0 + variance);
  
  // Efficiency: inverse of complexity
  const efficiency = 1.0 / Math.sqrt(kernel.order);
  
  // Novelty: genome diversity (based on lineage length)
  const novelty = Math.min(1.0, kernel.genome.lineage.length / 10.0);
  
  // Symmetry: from tree structures
  const avgSymmetry = kernel.trees.reduce((sum, t) => sum + t.symmetry, 0) / kernel.trees.length;
  const symmetry = Math.min(1.0, avgSymmetry / 6.0);
  
  return { grip, stability, efficiency, novelty, symmetry };
}

/**
 * Calculate overall fitness score
 */
export function calculateOverallFitness(metrics: FitnessMetrics): number {
  return (
    metrics.grip * 0.4 +
    metrics.stability * 0.2 +
    metrics.efficiency * 0.2 +
    metrics.novelty * 0.1 +
    metrics.symmetry * 0.1
  );
}

/**
 * Initialize an ontogenetic kernel from a base kernel
 */
export function initializeOntogeneticKernel(kernel: GeneratedKernel): OntogeneticKernel {
  const genes = kernelToGenes(kernel);
  
  const genome: KernelGenome = {
    id: generateGenomeId(),
    generation: 0,
    lineage: [],
    genes,
    fitness: 0,
    age: 0
  };
  
  const ontogeneticState: OntogeneticState = {
    stage: DevelopmentStage.Embryonic,
    maturity: 0.0,
    reproductiveCapacity: 0.0,
    developmentHistory: [{
      generation: 0,
      stage: DevelopmentStage.Embryonic,
      fitness: 0,
      timestamp: new Date(),
      description: 'Initial kernel creation'
    }]
  };
  
  const ontoKernel: OntogeneticKernel = {
    ...kernel,
    genome,
    ontogeneticState
  };
  
  // Calculate initial fitness
  const metrics = calculateFitness(ontoKernel);
  ontoKernel.genome.fitness = calculateOverallFitness(metrics);
  
  return ontoKernel;
}

/**
 * Self-generate: Create offspring through recursive self-composition
 * Uses chain rule: (f∘f)' = f'(f(x)) · f'(x)
 */
export function selfGenerate(parent: OntogeneticKernel): OntogeneticKernel {
  // Apply chain rule to coefficients
  // New coefficients are products of parent coefficients
  const newCoefficients = parent.coefficients.map((c, i) => {
    // Self-composition: multiply coefficient by itself and scale
    return c * c * parent.trees[i].symmetry / parent.trees[i].order;
  });
  
  // Normalize
  const sum = newCoefficients.reduce((a, b) => a + Math.abs(b), 0);
  const normalizedCoefficients = newCoefficients.map(c => c / sum);
  
  // Create offspring kernel
  const offspring: GeneratedKernel = {
    id: `${parent.id}_offspring`,
    domain: parent.domain,
    order: parent.order,
    trees: parent.trees,
    coefficients: normalizedCoefficients,
    grip: calculateGrip(normalizedCoefficients, parent.domain)
  };
  
  // Initialize as ontogenetic kernel
  const ontoOffspring = initializeOntogeneticKernel(offspring);
  
  // Update genome
  ontoOffspring.genome.generation = parent.genome.generation + 1;
  ontoOffspring.genome.lineage = [...parent.genome.lineage, parent.genome.id];
  ontoOffspring.genome.id = generateGenomeId();
  
  // Record development event
  ontoOffspring.ontogeneticState.developmentHistory.push({
    generation: ontoOffspring.genome.generation,
    stage: DevelopmentStage.Embryonic,
    fitness: ontoOffspring.genome.fitness,
    timestamp: new Date(),
    description: `Self-generated from parent ${parent.genome.id}`
  });
  
  return ontoOffspring;
}

/**
 * Self-optimize: Improve grip through iterative optimization
 */
export function selfOptimize(kernel: OntogeneticKernel, iterations: number = 10): OntogeneticKernel {
  let optimized = { ...kernel };
  const learningRate = 0.05;
  
  for (let iter = 0; iter < iterations; iter++) {
    // Gradient ascent on grip
    const currentGrip = optimized.grip;
    const newCoefficients = [...optimized.coefficients];
    
    // Try small perturbations
    for (let i = 0; i < newCoefficients.length; i++) {
      const delta = (Math.random() - 0.5) * learningRate;
      newCoefficients[i] += delta;
    }
    
    // Normalize
    const sum = newCoefficients.reduce((a, b) => a + Math.abs(b), 0);
    const normalizedCoefficients = newCoefficients.map(c => c / sum);
    
    // Check if improved
    const newGrip = calculateGrip(normalizedCoefficients, optimized.domain);
    if (newGrip > currentGrip) {
      optimized.coefficients = normalizedCoefficients;
      optimized.grip = newGrip;
      
      // Update maturity
      optimized.ontogeneticState.maturity = Math.min(
        1.0,
        optimized.ontogeneticState.maturity + 0.1
      );
      
      // Update stage based on maturity
      if (optimized.ontogeneticState.maturity >= 0.8) {
        optimized.ontogeneticState.stage = DevelopmentStage.Mature;
        optimized.ontogeneticState.reproductiveCapacity = 1.0;
      } else if (optimized.ontogeneticState.maturity >= 0.4) {
        optimized.ontogeneticState.stage = DevelopmentStage.Juvenile;
        optimized.ontogeneticState.reproductiveCapacity = 0.5;
      }
    }
  }
  
  // Recalculate fitness
  const metrics = calculateFitness(optimized);
  optimized.genome.fitness = calculateOverallFitness(metrics);
  
  // Record development event
  optimized.ontogeneticState.developmentHistory.push({
    generation: optimized.genome.generation,
    stage: optimized.ontogeneticState.stage,
    fitness: optimized.genome.fitness,
    timestamp: new Date(),
    description: `Self-optimized through ${iterations} iterations`
  });
  
  return optimized;
}

/**
 * Self-reproduce: Combine two kernels through genetic operations
 */
export function selfReproduce(
  parent1: OntogeneticKernel,
  parent2: OntogeneticKernel,
  method: ReproductionMethod = ReproductionMethod.Crossover
): OntogeneticKernel {
  let offspringCoefficients: number[];
  
  switch (method) {
    case ReproductionMethod.Crossover:
      // Single-point crossover
      const crossoverPoint = Math.floor(parent1.coefficients.length / 2);
      offspringCoefficients = [
        ...parent1.coefficients.slice(0, crossoverPoint),
        ...parent2.coefficients.slice(crossoverPoint)
      ];
      break;
      
    case ReproductionMethod.Mutation:
      // Random mutation of parent1
      offspringCoefficients = parent1.coefficients.map(c => 
        c + (Math.random() - 0.5) * 0.2
      );
      break;
      
    case ReproductionMethod.Cloning:
      // Direct copy
      offspringCoefficients = [...parent1.coefficients];
      break;
  }
  
  // Normalize
  const sum = offspringCoefficients.reduce((a, b) => a + Math.abs(b), 0);
  const normalizedCoefficients = offspringCoefficients.map(c => c / sum);
  
  // Create offspring
  const offspring: GeneratedKernel = {
    id: `${parent1.id}_x_${parent2.id}`,
    domain: parent1.domain,
    order: parent1.order,
    trees: parent1.trees,
    coefficients: normalizedCoefficients,
    grip: calculateGrip(normalizedCoefficients, parent1.domain)
  };
  
  // Initialize as ontogenetic kernel
  const ontoOffspring = initializeOntogeneticKernel(offspring);
  
  // Update genome
  ontoOffspring.genome.generation = Math.max(parent1.genome.generation, parent2.genome.generation) + 1;
  ontoOffspring.genome.lineage = [parent1.genome.id, parent2.genome.id];
  ontoOffspring.genome.id = generateGenomeId();
  
  // Record development event
  ontoOffspring.ontogeneticState.developmentHistory.push({
    generation: ontoOffspring.genome.generation,
    stage: DevelopmentStage.Embryonic,
    fitness: ontoOffspring.genome.fitness,
    timestamp: new Date(),
    description: `Reproduced via ${method} from parents ${parent1.genome.id} and ${parent2.genome.id}`
  });
  
  return ontoOffspring;
}
