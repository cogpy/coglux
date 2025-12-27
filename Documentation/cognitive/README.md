# OpenCog AGI-OS Cognitive Kernel

## Overview

The OpenCog AGI-OS cognitive kernel is a complete implementation of OpenCog's cognitive architecture directly integrated into the Linux kernel. This provides high-performance cognitive computing capabilities including knowledge representation, intelligent scheduling, goal-oriented process management, and learning adaptation.

## Key Features

### 1. AtomSpace Knowledge Representation
- **Hypergraph storage**: Knowledge stored as typed atoms (nodes and links) with truth values
- **UUID-based indexing**: Fast lookups using red-black trees
- **Memory management**: Kernel-space memory allocation with reference counting
- **Pattern matching**: Efficient query and pattern matching capabilities

### 2. Cognitive-Aware Scheduling
- **Attention allocation**: Tasks compete for attention units based on cognitive load
- **Goal-oriented priorities**: Tasks with higher-priority goals get scheduling preference
- **Learning adaptation**: Scheduler adapts based on learning patterns and performance
- **Multi-policy support**: Balanced, learning-focused, reasoning-focused, reactive, and goal-driven policies

### 3. BPF Scheduler Integration
- **sched_ext integration**: Uses Linux's extensible scheduler framework
- **Dynamic CPU selection**: Chooses CPUs based on available cognitive resources
- **Adaptive time slices**: Time allocation based on cognitive task priority
- **Runtime tracking**: Monitors task execution for adaptation scoring

### 4. System Call Interface
- **atom_create()**: Create knowledge atoms in kernel AtomSpace
- **atom_query()**: Query atoms by pattern matching
- **set_attrs()**: Configure cognitive attributes for processes
- **set_goal()**: Assign cognitive goals to processes
- **get_attention()**: Query available attention resources

### 5. Memory Management
- **Pattern-based optimization**: Detects cognitive access patterns
- **Adaptive prefetching**: Optimizes memory access for AI workloads
- **Learning-aware allocation**: Special handling for ML training vs inference

### 6. Debug and Monitoring
- **`/proc/cognitive/atomspace`**: AtomSpace statistics and atom counts
- **`/proc/cognitive/scheduler`**: Cognitive scheduler status and per-CPU info
- **`/proc/cognitive/tasks`**: List of active cognitive tasks

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Userspace Applications                   │
│  ┌─────────────────┐ ┌─────────────────┐ ┌──────────────┐  │
│  │   ML Training   │ │   AI Inference  │ │  Reasoning   │  │
│  │   Applications  │ │   Applications  │ │ Applications │  │
│  └─────────────────┘ └─────────────────┘ └──────────────┘  │
└──────────────────────┬──────────────────┬──────────────────┘
                       │                  │
           ┌───────────▼──────────────────▼───────────┐
           │        Cognitive System Calls           │
           └───────────┬──────────────────┬───────────┘
                       │                  │
┌──────────────────────▼──────────────────▼──────────────────┐
│                 Linux Kernel Space                        │
│                                                           │
│  ┌─────────────────────────────────────────────────────┐  │
│  │              Cognitive Subsystem                    │  │
│  │  ┌─────────────┐ ┌─────────────┐ ┌──────────────┐  │  │
│  │  │ AtomSpace   │ │  Scheduler  │ │   Memory     │  │  │
│  │  │  Manager    │ │  Manager    │ │   Manager    │  │  │
│  │  └─────────────┘ └─────────────┘ └──────────────┘  │  │
│  │  ┌─────────────┐ ┌─────────────┐ ┌──────────────┐  │  │
│  │  │  Pattern    │ │ BPF Sched   │ │   Debug      │  │  │
│  │  │  Matcher    │ │ Extension   │ │ Interface    │  │  │
│  │  └─────────────┘ └─────────────┘ └──────────────┘  │  │
│  └─────────────────────────────────────────────────────┘  │
│                                                           │
│  ┌─────────────────────────────────────────────────────┐  │
│  │            Standard Linux Subsystems                │  │
│  │     (Scheduler, MM, VFS, Network, etc.)            │  │
│  └─────────────────────────────────────────────────────┘  │
└───────────────────────────────────────────────────────────┘
```

## Configuration

Enable cognitive kernel features in kernel configuration:

```
CONFIG_COGNITIVE_KERNEL=y          # Enable cognitive kernel
CONFIG_COGNITIVE_ATOMSPACE=y       # AtomSpace in kernel
CONFIG_COGNITIVE_SCHEDULER=y       # Cognitive scheduler
CONFIG_COGNITIVE_MEMORY=y          # Cognitive memory manager
CONFIG_COGNITIVE_SYSCALLS=y        # Cognitive system calls
CONFIG_COGNITIVE_DEBUG=y           # Debug interfaces
```

## Usage Examples

### C Program Example

```c
#include <sys/syscall.h>
#include <unistd.h>

// Create a concept atom
long atom_uuid = syscall(__NR_cognitive_atom_create, 
                        ATOM_TYPE_CONCEPT, 
                        "learning_task", 
                        12);

// Set cognitive attributes
struct cognitive_task attrs = {
    .reasoning_load = 80,
    .learning_rate = 95,
    .goal_priority = 10
};
syscall(__NR_cognitive_set_attrs, &attrs);

// Set a goal
syscall(__NR_cognitive_set_goal, atom_uuid, 10);
```

### Monitoring Commands

```bash
# View AtomSpace statistics
cat /proc/cognitive/atomspace

# View scheduler status
cat /proc/cognitive/scheduler

# View cognitive tasks
cat /proc/cognitive/tasks
```

## Implementation Status

### Completed ✓
- [x] Core AtomSpace implementation with CRUD operations
- [x] Pattern matching engine with multiple pattern types
- [x] Cognitive scheduler with attention allocation
- [x] BPF scheduler extension with sched_ext integration
- [x] Complete system call interface
- [x] Comprehensive debug and monitoring interfaces
- [x] Cognitive memory manager foundation
- [x] Task structure integration
- [x] Kernel configuration and build system integration
- [x] **Ontogenesis framework** - Self-generating, evolving kernels (TypeScript library)
- [x] **Universal Kernel Generator** - Domain-specific kernel generation via B-series
- [x] **A000081 implementation** - Rooted trees for elementary differentials

### Future Enhancements
- [ ] Advanced pattern indexing for performance
- [ ] Distributed AtomSpace across NUMA nodes
- [ ] Machine learning framework integration
- [ ] Real-time cognitive constraints
- [ ] Performance benchmarking suite
- [ ] Kernel-space ontogenesis integration
- [ ] Hardware-accelerated kernel evolution

## Testing

Build and run the test program:

```bash
cd tools/cognitive
make
sudo ./test_cognitive
```

The test program verifies:
- System call availability
- AtomSpace operations
- Cognitive scheduling
- Debug interface access
- Learning scenario simulation

## Performance Considerations

- **Memory overhead**: AtomSpace requires additional memory per process
- **CPU overhead**: Cognitive scheduling adds computation to scheduler path
- **Attention management**: Limited attention budget requires careful allocation
- **Debug interfaces**: Disable in production for optimal performance

## Research Applications

The cognitive kernel is designed for:
- **AI/ML research**: Kernel-level optimization for training and inference
- **Cognitive computing**: Goal-oriented and attention-based computing
- **Adaptive systems**: Self-modifying and learning system behavior
- **AGI development**: Foundation for artificial general intelligence systems
- **Ontogenesis research**: Self-generating and evolving computational kernels

## Ontogenesis: Self-Generating Kernels

CogLux includes a revolutionary **ontogenesis system** that implements living mathematical structures. Computational kernels can:

- **Self-generate**: Create offspring through recursive composition (chain rule)
- **Self-optimize**: Improve their grip (fit to domain) through iterative optimization
- **Self-reproduce**: Combine genetic information through crossover and mutation
- **Evolve**: Populations of kernels evolve over generations to maximize fitness

The ontogenesis system is implemented as a TypeScript library in `lib/ontogenesis/` and uses:

- **B-series expansion**: Universal language for kernel generation
- **A000081 sequence**: Rooted trees as elementary differentials
- **Differential operators**: Chain rule, product rule, quotient rule
- **Genetic algorithms**: Tournament selection, crossover, mutation, elitism

### Quick Example

```typescript
import { 
  UniversalKernelGenerator,
  initializeOntogeneticKernel,
  selfGenerate,
  runOntogenesis
} from 'coglux-ontogenesis';

// Generate consciousness kernel
const kernel = UniversalKernelGenerator.generateConsciousnessKernel(4);

// Initialize for ontogenesis
const ontoKernel = initializeOntogeneticKernel(kernel);

// Self-generate offspring
const offspring = selfGenerate(ontoKernel);

// Evolve population
const result = runOntogenesis({
  evolution: {
    populationSize: 20,
    mutationRate: 0.15,
    crossoverRate: 0.8,
    elitismRate: 0.1,
    maxGenerations: 50,
    fitnessThreshold: 0.9,
    diversityPressure: 0.2
  },
  seedKernels: [kernel]
});
```

See [Ontogenesis Documentation](./ontogenesis.md) and [Library README](../../lib/ontogenesis/README.md) for details.

## Contributors

- CogLux Project Team
- OpenCog Community
- Linux Kernel Developers (scheduler extensions)

## License

GPL v2 - Same as Linux kernel