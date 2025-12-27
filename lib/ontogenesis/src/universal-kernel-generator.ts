// SPDX-License-Identifier: GPL-2.0
/**
 * Universal Kernel Generator
 * 
 * Generates domain-specific computational kernels using B-series expansion
 * and differential operators. This is the foundation for ontogenesis.
 */

import { 
  GeneratedKernel, 
  DomainSpec, 
  DomainType, 
  ElementaryDifferential,
  RootedTree
} from './types';
import { generateElementaryDifferentials } from './a000081';

/**
 * Generate unique kernel ID
 * Note: Uses timestamp + random for simplicity. For production use,
 * consider crypto.randomUUID() or a proper UUID library.
 */
function generateKernelId(): string {
  return `kernel_${Date.now()}_${Math.random().toString(36).substring(2, 11)}`;
}

/**
 * Calculate Butcher weights for B-series
 * These determine how much each elementary differential contributes
 */
function calculateButcherWeights(trees: RootedTree[], domain: DomainSpec): number[] {
  return trees.map((tree, i) => {
    // Base weight inversely proportional to tree order and symmetry
    let weight = 1.0 / (tree.order * tree.symmetry);
    
    // Domain-specific adjustments
    switch (domain.type) {
      case DomainType.Physics:
        // Physics favors symmetric structures (conservation laws)
        weight *= tree.symmetry / tree.order;
        break;
      case DomainType.Chemistry:
        // Chemistry favors balanced tree structures
        weight *= 1.0 / Math.sqrt(tree.order);
        break;
      case DomainType.Biology:
        // Biology favors complex, asymmetric structures
        weight *= tree.order / tree.symmetry;
        break;
      case DomainType.Computing:
        // Computing favors simple, efficient structures
        weight *= 1.0 / (tree.order * tree.order);
        break;
      case DomainType.Consciousness:
        // Consciousness favors self-referential structures
        weight *= Math.log(tree.order + 1);
        break;
    }
    
    return weight;
  });
}

/**
 * Generate initial B-series coefficients
 */
function generateBSeriesCoefficients(trees: RootedTree[], domain: DomainSpec): number[] {
  const weights = calculateButcherWeights(trees, domain);
  
  // Normalize weights to sum to 1
  const sum = weights.reduce((a, b) => a + b, 0);
  return weights.map(w => w / sum);
}

/**
 * Calculate grip metric - how well kernel fits the domain
 */
export function calculateGrip(coefficients: number[], domain: DomainSpec): number {
  if (coefficients.length === 0) return 0;
  
  // Grip measures optimal contact with domain topology
  // Higher grip = better fit
  
  // 1. Coefficient distribution (should not be too concentrated)
  const entropy = coefficients.reduce((sum, c) => {
    if (c > 0) return sum - c * Math.log(c);
    return sum;
  }, 0);
  const maxEntropy = Math.log(coefficients.length);
  const distributionScore = entropy / maxEntropy;
  
  // 2. Coefficient magnitude (should be balanced)
  const magnitudes = coefficients.map(c => Math.abs(c));
  const avgMagnitude = magnitudes.reduce((a, b) => a + b, 0) / magnitudes.length;
  const variance = magnitudes.reduce((sum, m) => sum + Math.pow(m - avgMagnitude, 2), 0) / magnitudes.length;
  const balanceScore = 1.0 / (1.0 + variance);
  
  // 3. Domain-specific requirements
  let domainScore = 0.5;
  switch (domain.type) {
    case DomainType.Physics:
      // Physics requires strong symmetry preservation
      domainScore = 0.7 + 0.3 * balanceScore;
      break;
    case DomainType.Consciousness:
      // Consciousness requires high complexity
      domainScore = 0.6 + 0.4 * distributionScore;
      break;
    default:
      domainScore = 0.5 * (distributionScore + balanceScore);
  }
  
  // Combine scores
  const grip = 0.4 * distributionScore + 0.3 * balanceScore + 0.3 * domainScore;
  return Math.max(0, Math.min(1, grip));
}

/**
 * Generate kernel for physics domain
 * Emphasizes Hamiltonian structure and symplectic geometry
 */
export function generatePhysicsKernel(order: number): GeneratedKernel {
  const domain: DomainSpec = {
    type: DomainType.Physics,
    order,
    symmetries: ['time-reversal', 'energy-conservation', 'momentum-conservation'],
    invariants: ['Hamiltonian', 'symplectic-form'],
    constraints: { preserveSymplecticStructure: true }
  };
  
  const trees = generateElementaryDifferentials(order);
  const coefficients = generateBSeriesCoefficients(trees, domain);
  const grip = calculateGrip(coefficients, domain);
  
  return {
    id: generateKernelId(),
    domain,
    order,
    trees,
    coefficients,
    grip
  };
}

/**
 * Generate kernel for chemistry domain
 * Emphasizes reaction networks and detailed balance
 */
export function generateChemistryKernel(order: number): GeneratedKernel {
  const domain: DomainSpec = {
    type: DomainType.Chemistry,
    order,
    symmetries: ['mass-conservation', 'charge-conservation'],
    invariants: ['reaction-rate', 'equilibrium-constant'],
    constraints: { detailedBalance: true }
  };
  
  const trees = generateElementaryDifferentials(order);
  const coefficients = generateBSeriesCoefficients(trees, domain);
  const grip = calculateGrip(coefficients, domain);
  
  return {
    id: generateKernelId(),
    domain,
    order,
    trees,
    coefficients,
    grip
  };
}

/**
 * Generate kernel for biology domain
 * Emphasizes metabolic networks and homeostasis
 */
export function generateBiologyKernel(order: number): GeneratedKernel {
  const domain: DomainSpec = {
    type: DomainType.Biology,
    order,
    symmetries: ['homeostasis'],
    invariants: ['metabolic-flux', 'ATP-production'],
    constraints: { maintainHomeostasis: true }
  };
  
  const trees = generateElementaryDifferentials(order);
  const coefficients = generateBSeriesCoefficients(trees, domain);
  const grip = calculateGrip(coefficients, domain);
  
  return {
    id: generateKernelId(),
    domain,
    order,
    trees,
    coefficients,
    grip
  };
}

/**
 * Generate kernel for computing domain
 * Emphasizes recursion and Church-Rosser properties
 */
export function generateComputingKernel(order: number): GeneratedKernel {
  const domain: DomainSpec = {
    type: DomainType.Computing,
    order,
    symmetries: ['commutativity', 'associativity'],
    invariants: ['Church-Rosser', 'termination'],
    constraints: { preserveChurchRosser: true }
  };
  
  const trees = generateElementaryDifferentials(order);
  const coefficients = generateBSeriesCoefficients(trees, domain);
  const grip = calculateGrip(coefficients, domain);
  
  return {
    id: generateKernelId(),
    domain,
    order,
    trees,
    coefficients,
    grip
  };
}

/**
 * Generate kernel for consciousness domain
 * Emphasizes self-reference and echo state networks
 */
export function generateConsciousnessKernel(order: number): GeneratedKernel {
  const domain: DomainSpec = {
    type: DomainType.Consciousness,
    order,
    symmetries: ['self-reference', 'recursion'],
    invariants: ['echo-state', 'attention-flow'],
    constraints: { preserveSelfReference: true }
  };
  
  const trees = generateElementaryDifferentials(order);
  const coefficients = generateBSeriesCoefficients(trees, domain);
  const grip = calculateGrip(coefficients, domain);
  
  return {
    id: generateKernelId(),
    domain,
    order,
    trees,
    coefficients,
    grip
  };
}

/**
 * Generate custom kernel from domain specification
 */
export function generateCustomKernel(domain: DomainSpec): GeneratedKernel {
  const trees = generateElementaryDifferentials(domain.order);
  const coefficients = generateBSeriesCoefficients(trees, domain);
  const grip = calculateGrip(coefficients, domain);
  
  return {
    id: generateKernelId(),
    domain,
    order: domain.order,
    trees,
    coefficients,
    grip
  };
}

/**
 * Universal Kernel Generator - main export
 */
export const UniversalKernelGenerator = {
  generatePhysicsKernel,
  generateChemistryKernel,
  generateBiologyKernel,
  generateComputingKernel,
  generateConsciousnessKernel,
  generateCustomKernel,
  calculateGrip
};
