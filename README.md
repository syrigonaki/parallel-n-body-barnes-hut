# Barnes-Hut N-Body Simulation - Parallel Implementation with TBB
Uses Intel TBB for parallelizing the computation of forces and position updates.

## Compilation

### Using g++

```bash
g++ -std=c++17 barnes_hut.cpp -o barnes_hut -ltbb -ltbbmalloc
```

### Using Makefile

```bash
make all
```

Remove executable and temporary files using

```bash
make clean
```

## Running the program

```bash
./barnes_hut <input_file> <num_iterations> <output_file>
```
input_file format:
* first line: number of bodies
* second line: space dimension
* next lines: x y vx vy mass name for each body

num_iterations is the number of simulation steps to perform.

output_file contains the updated positions, velocities, and masses of all bodies.



## Notes:

* Default number of threads is defined by 'THREADS_N'. Can be adjusted for parallel performance.
* Uses a Barnes-Hut quadtree for efficient force computation (O(N log N) complexity).
* Positions and velocities are updated in parallel.
* Reconstructs the quadtree each iteration to account for movement of bodies.
* Execution time is printed at the end of the simulation.
---
  *This was developed as an assignment for the Parallel Programming Course*
