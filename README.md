# Maze Solver 

> **C++ Console Application**  
> Graph Traversal with **DFS** (Recursive & Iterative) and **BFS**

## Project Description

This project implements a maze solver using three graph traversal algorithms in C++:

- **DFS – Recursive**: Uses the call stack; backtracks on dead ends.
- **DFS – Iterative**: Same strategy but with an explicit `std::stack`.
- **BFS**: Uses `std::queue`; **guarantees the shortest path**.

The maze is stored as a 2D grid of characters:

| Character | Meaning        |
|-----------|----------------|
| `#`       | Wall           |
| ` `       | Passable cell  |
| `S`       | Start          |
| `E`       | Exit           |
| `*`       | Solution path  |

---

## How to Compile & Run

### Requirements
- C++17 or later (`g++ -std=c++17`)
- Linux / macOS / Windows (ANSI colours work on modern terminals)

### Compile
```bash
g++ -std=c++17 -O2 -o maze_solver maze_solver.cpp
```

### Run
```bash
./maze_solver
```

---

## Features

- **Coloured console output** — walls, start, exit, and solution path are colour-coded.
- **Four predefined mazes** — small, medium, no-solution, and open-grid.
- **Custom maze builder** — enter your own maze row by row.
- **Step-by-step direction log** — e.g. `Right → Down → Down → Right`.
- **Elapsed time** measurement per algorithm.
- **Complexity analysis** table built into the menu.

---

## Complexity Summary

| Algorithm       | Time    | Space      | Shortest Path? |
|-----------------|---------|------------|----------------|
| DFS (Recursive) | O(V+E)  | O(V) stack | No             |
| DFS (Iterative) | O(V+E)  | O(V) stack | No             |
| BFS             | O(V+E)  | O(V) queue | **Yes ✓**      |

> V = total cells in grid, E = passable connections (≤ 4V)

---

## Project Structure

```
maze_solver/
├── maze_solver.cpp   ← Main source file (all algorithms + UI)
└── README.md
