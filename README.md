# Pseudo-Boolean to CNF Translator

This project implements a translator for **pseudo-Boolean constraints** (and their special case, **cardinality constraints**) into **Conjunctive Normal Form (CNF)** suitable for SAT solvers.

## Features
- Parses pseudo-Boolean constraints from text input.  
- Normalizes constraints into canonical form.  
- Encodes:
  - **Cardinality constraints** using **Sequential Counter Encoding**.  
  - **General pseudo-Boolean constraints** using **Adder Network Encoding**.  
- Outputs formulas in **DIMACS CNF format** for use with solvers like MiniSat.  

## Implementation
- Written in **C++17**.  
- Organized into modules for:
  - Constraint parsing and normalization.  
  - CNF representation and export.  
  - Encoding techniques (sequential counter, adder network).  
- Build system: **CMake (>= 3.10)**.  

## Build & Run
```bash
mkdir build
cd build
cmake ..
make
```

Run the program from the command line, providing constraints via standard input:
```bash
./sat_encoder
x1 + x2 + x3 >= 3
¬x1 - x2 > 1
```

The CNF clauses are written to `output.cnf`. Test cases are available in the `tests/` directory.

## References
- [MiniSat+ Documentation](http://minisat.se/downloads/MiniSat+.pdf)  
- [Carsten Sinz, *Towards an Optimal CNF Encoding of Boolean Cardinality Constraints*](https://www.carstensinz.de/papers/CP-2005.pdf)  
- [Ignasi Abío et al., *Encoding Linear Constraints into SAT*](https://arxiv.org/pdf/2005.02073)  
- [Jakob Nordström, *Pseudo-Boolean Solving and Optimization Tutorial*](https://simons.berkeley.edu/sites/default/files/docs/16969/pseudobooleansolvingtutorial.pdf)  
