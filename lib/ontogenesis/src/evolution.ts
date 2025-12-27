// SPDX-License-Identifier: GPL-2.0
/**
 * Evolution Engine
 * 
 * Implements population-based genetic algorithm for evolving ontogenetic kernels.
 * Uses tournament selection, crossover, mutation, and elitism.
 */

import {
  OntogeneticKernel,
  OntogenesisConfig,
  GenerationStats,
  EvolutionResult,
  ReproductionMethod
} from './types';
import {
  initializeOntogeneticKernel,
  selfReproduce,
  selfOptimize,
  calculateFitness,
  calculateOverallFitness
} from './ontogenetic-operations';

/**
 * Tournament selection - select parent based on fitness
 */
function tournamentSelection(
  population: OntogeneticKernel[],
  tournamentSize: number = 3
): OntogeneticKernel {
  // Randomly select individuals for tournament
  const tournament: OntogeneticKernel[] = [];
  for (let i = 0; i < tournamentSize; i++) {
    const idx = Math.floor(Math.random() * population.length);
    tournament.push(population[idx]);
  }
  
  // Return fittest individual
  tournament.sort((a, b) => b.genome.fitness - a.genome.fitness);
  return tournament[0];
}

/**
 * Calculate population diversity
 */
function calculateDiversity(population: OntogeneticKernel[]): number {
  if (population.length <= 1) return 0;
  
  // Measure coefficient diversity
  let totalDistance = 0;
  let comparisons = 0;
  
  for (let i = 0; i < population.length; i++) {
    for (let j = i + 1; j < population.length; j++) {
      // Euclidean distance between coefficient vectors
      let distance = 0;
      for (let k = 0; k < population[i].coefficients.length; k++) {
        distance += Math.pow(population[i].coefficients[k] - population[j].coefficients[k], 2);
      }
      totalDistance += Math.sqrt(distance);
      comparisons++;
    }
  }
  
  return comparisons > 0 ? totalDistance / comparisons : 0;
}

/**
 * Initialize population from seed kernels
 */
function initializePopulation(config: OntogenesisConfig): OntogeneticKernel[] {
  const population: OntogeneticKernel[] = [];
  
  // Convert seed kernels to ontogenetic kernels
  for (const seedKernel of config.seedKernels) {
    const ontoKernel = initializeOntogeneticKernel(seedKernel);
    population.push(ontoKernel);
  }
  
  // Generate additional individuals to reach population size
  while (population.length < config.evolution.populationSize) {
    // Pick random seed and mutate
    const seed = config.seedKernels[Math.floor(Math.random() * config.seedKernels.length)];
    const ontoKernel = initializeOntogeneticKernel(seed);
    
    // Apply random mutation
    const mutated = selfReproduce(ontoKernel, ontoKernel, ReproductionMethod.Mutation);
    population.push(mutated);
  }
  
  return population;
}

/**
 * Evolve population for one generation
 */
function evolveGeneration(
  population: OntogeneticKernel[],
  config: OntogenesisConfig,
  generation: number
): OntogeneticKernel[] {
  const { evolution } = config;
  const newPopulation: OntogeneticKernel[] = [];
  
  // Elitism: keep top performers
  const eliteCount = Math.floor(evolution.populationSize * evolution.elitismRate);
  const sortedPopulation = [...population].sort((a, b) => b.genome.fitness - a.genome.fitness);
  
  for (let i = 0; i < eliteCount; i++) {
    const elite = { ...sortedPopulation[i] };
    elite.genome.age++;
    newPopulation.push(elite);
  }
  
  // Generate offspring to fill population
  while (newPopulation.length < evolution.populationSize) {
    const parent1 = tournamentSelection(population);
    const parent2 = tournamentSelection(population);
    
    let offspring: OntogeneticKernel;
    
    // Crossover
    if (Math.random() < evolution.crossoverRate) {
      offspring = selfReproduce(parent1, parent2, ReproductionMethod.Crossover);
    } else {
      offspring = selfReproduce(parent1, parent1, ReproductionMethod.Cloning);
    }
    
    // Mutation
    if (Math.random() < evolution.mutationRate) {
      offspring = selfReproduce(offspring, offspring, ReproductionMethod.Mutation);
    }
    
    // Self-optimization for some individuals
    if (Math.random() < 0.3) {
      offspring = selfOptimize(offspring, 5);
    }
    
    // Recalculate fitness
    const metrics = calculateFitness(offspring);
    offspring.genome.fitness = calculateOverallFitness(metrics);
    offspring.genome.generation = generation;
    
    newPopulation.push(offspring);
  }
  
  return newPopulation;
}

/**
 * Calculate generation statistics
 */
function calculateGenerationStats(
  population: OntogeneticKernel[],
  generation: number
): GenerationStats {
  const fitnesses = population.map(k => k.genome.fitness);
  const bestFitness = Math.max(...fitnesses);
  const worstFitness = Math.min(...fitnesses);
  const averageFitness = fitnesses.reduce((a, b) => a + b, 0) / fitnesses.length;
  const diversity = calculateDiversity(population);
  
  const bestKernel = population.find(k => k.genome.fitness === bestFitness)!;
  
  return {
    generation,
    populationSize: population.length,
    bestFitness,
    averageFitness,
    worstFitness,
    diversity,
    bestKernel
  };
}

/**
 * Run ontogenesis: evolve population over multiple generations
 */
export function runOntogenesis(config: OntogenesisConfig): EvolutionResult {
  console.log('Starting ontogenesis evolution...');
  console.log(`Population size: ${config.evolution.populationSize}`);
  console.log(`Max generations: ${config.evolution.maxGenerations}`);
  
  // Initialize population
  let population = initializePopulation(config);
  const generationStats: GenerationStats[] = [];
  let convergenceGeneration = -1;
  
  // Evolve over generations
  for (let gen = 0; gen < config.evolution.maxGenerations; gen++) {
    // Calculate statistics
    const stats = calculateGenerationStats(population, gen);
    generationStats.push(stats);
    
    console.log(
      `Generation ${gen}: ` +
      `Best=${stats.bestFitness.toFixed(4)}, ` +
      `Avg=${stats.averageFitness.toFixed(4)}, ` +
      `Diversity=${stats.diversity.toFixed(4)}`
    );
    
    // Check convergence
    if (stats.bestFitness >= config.evolution.fitnessThreshold) {
      convergenceGeneration = gen;
      console.log(`Converged at generation ${gen}!`);
      break;
    }
    
    // Evolve to next generation
    population = evolveGeneration(population, config, gen + 1);
  }
  
  // Final statistics
  const finalGen = generationStats[generationStats.length - 1].generation;
  const finalStats = calculateGenerationStats(population, finalGen);
  
  console.log('Evolution complete!');
  console.log(`Final best fitness: ${finalStats.bestFitness.toFixed(4)}`);
  console.log(`Total generations: ${generationStats.length}`);
  
  return {
    generations: generationStats,
    finalPopulation: population,
    convergenceGeneration,
    totalGenerations: generationStats.length
  };
}

/**
 * Run ontogenesis with progress callback
 */
export function runOntogenesisWithCallback(
  config: OntogenesisConfig,
  onGenerationComplete: (stats: GenerationStats) => void
): EvolutionResult {
  // Initialize population
  let population = initializePopulation(config);
  const generationStats: GenerationStats[] = [];
  let convergenceGeneration = -1;
  
  // Evolve over generations
  for (let gen = 0; gen < config.evolution.maxGenerations; gen++) {
    // Calculate statistics
    const stats = calculateGenerationStats(population, gen);
    generationStats.push(stats);
    
    // Callback
    onGenerationComplete(stats);
    
    // Check convergence
    if (stats.bestFitness >= config.evolution.fitnessThreshold) {
      convergenceGeneration = gen;
      break;
    }
    
    // Evolve to next generation
    population = evolveGeneration(population, config, gen + 1);
  }
  
  // Final statistics
  const finalGen = generationStats[generationStats.length - 1].generation;
  const finalStats = calculateGenerationStats(population, finalGen);
  
  return {
    generations: generationStats,
    finalPopulation: population,
    convergenceGeneration,
    totalGenerations: generationStats.length
  };
}
