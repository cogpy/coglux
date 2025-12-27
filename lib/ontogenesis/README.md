# CogLux Ontogenesis

Self-generating, evolving computational kernels using differential calculus and B-series expansion.

## Overview

Ontogenesis implements **living mathematical structures** that exhibit life-like properties:

- **Self-replication**: Generate copies with variation through recursive composition
- **Evolution**: Improve through selection pressure and genetic algorithms
- **Development**: Progress through life stages (embryonic → juvenile → mature → senescent)
- **Reproduction**: Combine genetic information through crossover and mutation
- **Adaptation**: Optimize themselves for specific computational domains

## Key Concepts

### Universal Kernel Generator

The Universal Kernel Generator uses **B-series expansion** as a universal language for generating domain-specific computational kernels:

```
y_{n+1} = y_n + h·Σ b_i·Φ_i(f, y_n)
```

Where:
- `b_i`: Coefficient genes (mutable)
- `Φ_i`: Elementary differentials (rooted trees from A000081 sequence)

### A000081 Sequence

The A000081 OEIS sequence counts rooted trees, forming the basis for elementary differentials:

```
0, 1, 1, 2, 4, 9, 20, 48, 115, 286, 719, 1842...
```

Each tree represents an elementary differential operator in the B-series.

### Differential Operators

Kernels reproduce through differential calculus:

1. **Chain Rule** (Self-composition): `(f∘f)' = f'(f(x)) · f'(x)`
2. **Product Rule** (Parallel combination): `(f·g)' = f'·g + f·g'`
3. **Quotient Rule** (Ratio refinement): `(f/g)' = (f'·g - f·g')/g²`

## Installation

```bash
cd lib/ontogenesis
npm install
npm run build
```

## Usage

### Basic Example

```typescript
import { 
  UniversalKernelGenerator, 
  initializeOntogeneticKernel,
  selfGenerate 
} from 'coglux-ontogenesis';

// Generate a consciousness kernel
const kernel = UniversalKernelGenerator.generateConsciousnessKernel(4);

// Initialize for ontogenesis
const ontoKernel = initializeOntogeneticKernel(kernel);

// Self-generate offspring through recursive composition
const offspring = selfGenerate(ontoKernel);

console.log('Parent:', ontoKernel.genome.id);
console.log('Offspring:', offspring.genome.id);
console.log('Generation:', offspring.genome.generation);
console.log('Fitness:', offspring.genome.fitness);
```

### Self-Optimization

```typescript
import { selfOptimize } from 'coglux-ontogenesis';

// Optimize kernel through iterative grip improvement
const optimized = selfOptimize(ontoKernel, 20);

console.log('Original fitness:', ontoKernel.genome.fitness);
console.log('Optimized fitness:', optimized.genome.fitness);
console.log('Maturity:', optimized.ontogeneticState.maturity);
```

### Evolution

```typescript
import { runOntogenesis, UniversalKernelGenerator } from 'coglux-ontogenesis';

const config = {
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

const result = runOntogenesis(config);

// Analyze evolutionary progress
result.generations.forEach(gen => {
  console.log(`Generation ${gen.generation}:`);
  console.log(`  Best Fitness: ${gen.bestFitness.toFixed(4)}`);
  console.log(`  Average Fitness: ${gen.averageFitness.toFixed(4)}`);
  console.log(`  Diversity: ${gen.diversity.toFixed(4)}`);
});

console.log(`\nConverged at generation: ${result.convergenceGeneration}`);
console.log(`Total generations: ${result.totalGenerations}`);
```

### Domain-Specific Kernels

```typescript
import { UniversalKernelGenerator } from 'coglux-ontogenesis';

// Physics kernel (Hamiltonian, symplectic)
const physics = UniversalKernelGenerator.generatePhysicsKernel(4);

// Chemistry kernel (reaction networks, detailed balance)
const chemistry = UniversalKernelGenerator.generateChemistryKernel(4);

// Biology kernel (metabolic networks, homeostasis)
const biology = UniversalKernelGenerator.generateBiologyKernel(4);

// Computing kernel (recursion, Church-Rosser)
const computing = UniversalKernelGenerator.generateComputingKernel(4);

// Consciousness kernel (self-reference, echo state)
const consciousness = UniversalKernelGenerator.generateConsciousnessKernel(4);
```

## API Reference

### UniversalKernelGenerator

- `generatePhysicsKernel(order: number)`: Generate physics-specific kernel
- `generateChemistryKernel(order: number)`: Generate chemistry-specific kernel
- `generateBiologyKernel(order: number)`: Generate biology-specific kernel
- `generateComputingKernel(order: number)`: Generate computing-specific kernel
- `generateConsciousnessKernel(order: number)`: Generate consciousness-specific kernel
- `generateCustomKernel(domain: DomainSpec)`: Generate custom kernel from spec

### Ontogenetic Operations

- `initializeOntogeneticKernel(kernel: GeneratedKernel)`: Convert to ontogenetic kernel
- `selfGenerate(parent: OntogeneticKernel)`: Generate offspring through chain rule
- `selfOptimize(kernel: OntogeneticKernel, iterations: number)`: Optimize grip
- `selfReproduce(parent1, parent2, method)`: Genetic reproduction
- `calculateFitness(kernel: OntogeneticKernel)`: Calculate fitness metrics

### Evolution

- `runOntogenesis(config: OntogenesisConfig)`: Run multi-generation evolution
- `runOntogenesisWithCallback(config, callback)`: Run with progress callbacks

## Fitness Evaluation

Fitness is calculated from multiple metrics:

```
fitness = grip * 0.4 + stability * 0.2 + efficiency * 0.2 + novelty * 0.1 + symmetry * 0.1
```

- **Grip**: How well kernel fits domain topology [0, 1]
- **Stability**: Numerical stability (low coefficient variance) [0, 1]
- **Efficiency**: Computational efficiency (inverse of complexity) [0, 1]
- **Novelty**: Genetic diversity (lineage length) [0, 1]
- **Symmetry**: Structural symmetry (from tree structures) [0, 1]

## Development Stages

Kernels progress through life stages:

1. **Embryonic**: Just generated, basic structure (maturity < 0.4)
2. **Juvenile**: Developing, optimizing (maturity 0.4 - 0.8)
3. **Mature**: Fully developed, capable of reproduction (maturity ≥ 0.8)
4. **Senescent**: Declining, ready for replacement (not currently implemented)

## Performance

Typical evolution converges in 20-50 generations with:
- Population size: 20-50
- Mutation rate: 0.1-0.2
- Crossover rate: 0.7-0.9

## Mathematical Foundations

Based on:
- Butcher, J.C. (2016). *Numerical Methods for Ordinary Differential Equations*
- Hairer, E., et al. (1993). *Solving Ordinary Differential Equations I*
- OEIS A000081: https://oeis.org/A000081

## License

GPL-2.0 (same as Linux kernel)

## See Also

- [CogLux Cognitive Kernel](../../Documentation/cognitive/)
- [A000081 OEIS Documentation](../../.github/agents/A000081-OEIS.md)
- [Universal Kernel Generator](../../.github/agents/universal-kernel-generator.md)
- [Ontogenesis Documentation](../../.github/agents/ONTOGENESIS.md)
