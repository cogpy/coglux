// SPDX-License-Identifier: GPL-2.0
/**
 * Core type definitions for Ontogenesis - Self-generating kernels
 * 
 * Ontogenesis implements living mathematical structures that can generate
 * themselves, optimize themselves, reproduce, and evolve through generations.
 */

/**
 * Development stage of an ontogenetic kernel
 */
export enum DevelopmentStage {
  Embryonic = 'embryonic',  // Just generated, basic structure
  Juvenile = 'juvenile',    // Developing, optimizing
  Mature = 'mature',        // Fully developed, capable of reproduction
  Senescent = 'senescent'   // Declining, ready for replacement
}

/**
 * Type of genetic operation
 */
export enum ReproductionMethod {
  Crossover = 'crossover',  // Single-point genetic crossover
  Mutation = 'mutation',    // Random coefficient mutation
  Cloning = 'cloning'       // Direct copy
}

/**
 * A000081 rooted tree representation
 */
export interface RootedTree {
  id: string;
  order: number;              // Number of nodes
  structure: string;          // Tree structure representation
  symmetry: number;           // Symmetry factor
}

/**
 * Elementary differential based on rooted trees (A000081 sequence)
 */
export interface ElementaryDifferential {
  tree: RootedTree;
  coefficient: number;        // B-series coefficient
  weight: number;             // Butcher weight
}

/**
 * Kernel gene encoding differential operator information
 */
export interface KernelGene {
  id: string;
  differential: ElementaryDifferential;
  expression: number[];       // Coefficient array
  dominance: number;          // Gene strength [0, 1]
  mutable: boolean;           // Whether gene can mutate
}

/**
 * Kernel genome - the "DNA" of a computational kernel
 */
export interface KernelGenome {
  id: string;                 // Unique identifier
  generation: number;         // Generation number
  lineage: string[];          // Parent IDs
  genes: KernelGene[];        // Genetic information
  fitness: number;            // Overall fitness score
  age: number;                // Age in generations
}

/**
 * Development event in kernel ontogenesis
 */
export interface DevelopmentEvent {
  generation: number;
  stage: DevelopmentStage;
  fitness: number;
  timestamp: Date;
  description: string;
}

/**
 * State tracking for ontogenetic development
 */
export interface OntogeneticState {
  stage: DevelopmentStage;
  maturity: number;           // [0, 1] - 0 = embryonic, 1 = mature
  reproductiveCapacity: number; // [0, 1] - ability to reproduce
  developmentHistory: DevelopmentEvent[];
}

/**
 * Fitness components for kernel evaluation
 */
export interface FitnessMetrics {
  grip: number;               // How well kernel fits domain [0, 1]
  stability: number;          // Numerical stability [0, 1]
  efficiency: number;         // Computational efficiency [0, 1]
  novelty: number;            // Genetic diversity [0, 1]
  symmetry: number;           // Structural symmetry [0, 1]
}

/**
 * Domain specification for kernel generation
 */
export enum DomainType {
  Physics = 'physics',
  Chemistry = 'chemistry',
  Biology = 'biology',
  Computing = 'computing',
  Consciousness = 'consciousness',
  Custom = 'custom'
}

/**
 * Domain-specific kernel configuration
 */
export interface DomainSpec {
  type: DomainType;
  order: number;              // Differential order (complexity)
  symmetries: string[];       // Required symmetries
  invariants: string[];       // Conservation laws
  constraints: Record<string, any>;
}

/**
 * Base kernel structure from universal generator
 */
export interface GeneratedKernel {
  id: string;
  domain: DomainSpec;
  order: number;
  trees: RootedTree[];
  coefficients: number[];
  grip: number;
}

/**
 * Ontogenetic kernel with genetic capabilities
 */
export interface OntogeneticKernel extends GeneratedKernel {
  genome: KernelGenome;
  ontogeneticState: OntogeneticState;
}

/**
 * Evolution configuration
 */
export interface EvolutionConfig {
  populationSize: number;
  mutationRate: number;       // [0, 1] probability of mutation
  crossoverRate: number;      // [0, 1] probability of crossover
  elitismRate: number;        // [0, 1] fraction of top performers to keep
  maxGenerations: number;
  fitnessThreshold: number;   // Stop if best fitness exceeds this
  diversityPressure: number;  // [0, 1] importance of genetic diversity
}

/**
 * Ontogenesis configuration
 */
export interface OntogenesisConfig {
  evolution: EvolutionConfig;
  seedKernels: GeneratedKernel[];
  domainSpec?: DomainSpec;
}

/**
 * Generation statistics
 */
export interface GenerationStats {
  generation: number;
  populationSize: number;
  bestFitness: number;
  averageFitness: number;
  worstFitness: number;
  diversity: number;
  bestKernel: OntogeneticKernel;
}

/**
 * Evolution result
 */
export interface EvolutionResult {
  generations: GenerationStats[];
  finalPopulation: OntogeneticKernel[];
  convergenceGeneration: number;
  totalGenerations: number;
}
