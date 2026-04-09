# Barnes-Hut N-Body Simulation

C++/Java implementation of an N-body gravitational simulation using the Barnes-Hut algorithm.  

## Parallel Implementation with TBB
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

## Parallel Implementation with Java Threads

Uses Java Threads for parallelizing the computation of forces and position updates.

## Compilation

### Using g++

```bash
javac barnes_hut.java BHTree.java
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
java barnes_hut.java <input file> <num of iterations> <output file>
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
