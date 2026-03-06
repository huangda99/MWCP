# Efficient Computation of Maximum Weighted Clique in Dynamic Weighted Graph


## Description

Maximum weighted clique has important applications in social network analysis, bioinformatics, computer vision, and pattern recognition. Existing studies mainly focus on static weighted graphs. However, in real-world scenarios, graph structures often evolve over time (e.g., formation and dissolution of user relationships, changes in protein interactions, frequent updates in transaction networks). In such dynamic graphs, traditional static algorithms incur high computational costs and fail to meet real-time and scalability requirements.

This repository implements **PTWMC**, a prefix-tree-based weighted maximal clique index that addresses these challenges. The index organizes maximal cliques through prefix sharing, which enables rapid localization of affected index regions under dynamic updates and supports efficient computation of the weights of related vertices and cliques. By mapping edge weights to vertex weights, the framework handles both **maximum vertex-weighted clique** and **maximum edge-weighted clique** problems in a unified manner. The implementation includes:

- **Index construction**: Enumerate all maximal cliques and build the prefix-tree index.
- **Edge insertion maintenance**: Update the index when new edges are added.
- **Edge deletion maintenance**: Update the index when edges are removed.

The time complexity of the dynamic algorithms is significantly lower than the global recomputation cost of static methods and mainly depends on the size of the affected maximal clique set.

---

## Dataset Information

### Graph file format (DIMACS)

The input graph must be in **DIMACS format**:

- **Problem line**: `p edge <n> <m>` — `n` is the number of vertices, `m` is the number of edges. Vertices are labeled from `1` to `n`.
- **Edge lines**: `e <u> <v> <w>` — an undirected edge between vertices `u` and `v` with weight `w`.
- **Vertex lines** (optional): `n <id> <w>` — vertex `id` with vertex weight `w`.

Lines starting with `c` or `C` are treated as comments and ignored. Blank lines are also ignored.

**Example**:

```
p edge 7995 12264
e 5 4 2
e 5 1 1
e 4 2 5
n 1 0
n 2 0
n 3 0
n 4 0
n 5 0
```

### Edge update file format (for add/delete modes)

When using `add` or `delete` mode, the program reads an **edge update file**:

- One edge per line: `u v w` (vertex IDs and weight, whitespace-separated). Vertex IDs should be **0-based** (matching the internal representation after reading the DIMACS file).
- For **add**: each line is an edge to insert (must not already exist).
- For **delete**: each line is an edge to remove (must exist in the current graph).

Example files are provided in the `datasets/` directory (e.g. `uai_add.txt`, `uai_delete.txt`).

---

## Code Information

- **Language**: C++ (C++11).
- **Build system**: Make (see `MakeFile` in the project root).
- **Main components**:
  - `main.cpp`: Entry point; reads graph and invokes index build and optional maintenance.
  - `bicliqueIndex.cpp` / `bicliqueIndex.h`: Prefix tree structure and edge add/delete maintenance.
  - `degeneracy_algorithm_cliques_A.cpp` / `degeneracy_algorithm_cliques_A.h`: Degeneracy ordering and maximal clique enumeration.
  - `degeneracy_helper.cpp` / `degeneracy_helper.h`: Helpers for degeneracy-based enumeration.
  - `misc.cpp` / `misc.h`: Graph I/O (DIMACS), statistics, and orchestration.
  - `LinkedList.cpp` / `LinkedList.h`: Adjacency list and linked list utilities.
  - `MemoryManager.cpp` / `MemoryManager.h`: Custom memory allocation utilities.

---

## Usage Instructions

### 1. Compilation

In the project root directory (the directory containing `MakeFile`), run:

```bash
make
```

After successful compilation, the executable is:

- `bIndex.out`

To remove object files and the executable:

```bash
make clean
```

### 2. Running the program

The executable is invoked as:

```bash
./bIndex.out <graph_file_path> <add|delete|none> <edge_update_file_path>
```

**Parameters**

| Argument | Meaning |
|----------|--------|
| `graph_file_path` | Path to the input graph file in DIMACS format. |
| `add` \| `delete` \| `none` | **add**: build index then run edge insertion maintenance; **delete**: build index then run edge deletion maintenance; **none**: only build the index (no maintenance). |
| `edge_update_file_path` | Path to the file listing edges to add or delete. For `none`, this argument is ignored (you may use any placeholder, e.g. `_`). |

**Examples**

```bash
# Only build the maximal clique index
./bIndex.out datasets/uai_dimacs.txt none _

# Build the index, then perform edge insertion maintenance using edges from uai_add.txt
./bIndex.out datasets/uai_dimacs.txt add datasets/uai_add.txt

# Build the index, then perform edge deletion maintenance using edges from uai_delete.txt
./bIndex.out datasets/uai_dimacs.txt delete datasets/uai_delete.txt
```

### 3. Output

During execution the program prints:

- Graph statistics (e.g. number of vertices, edges, average degree).
- Index construction time and graph degeneracy.
- Memory usage of the prefix-tree index.
- When `add` or `delete` is used: runtime of the maintenance step and index memory usage after the update.

---

## Requirements

- **Compiler**: A C++11-compatible compiler (e.g. `g++`). The provided `MakeFile` uses `g++` with `-std=c++11 -Wall`.
- **Operating system**: Standard Unix-like environment (e.g. Linux) with `make`. On Windows, use WSL, MinGW, or a similar environment that provides `make` and a POSIX-style shell.

No external libraries beyond the C++ standard library are required.

---

## Methodology (if applicable)

1. **Graph loading**: The graph is read from the input file in DIMACS format into an adjacency list with edge and vertex weights.
2. **Degeneracy ordering**: A degeneracy ordering of the vertices is computed; this ordering is used to bound the search space during clique enumeration.
3. **Maximal clique enumeration**: All maximal cliques are enumerated using a recursive backtracking procedure guided by the degeneracy order (Bron–Kerbosch–style with pivot and degeneracy-based pruning).
4. **Prefix tree index**: Each maximal clique (as a sorted vertex set) is inserted into a prefix tree; shared prefixes are merged to save space.
5. **Edge insertion maintenance**: For a new edge (u,v), the code identifies affected branches in the prefix tree, computes new maximal cliques that include (u,v), and updates the tree accordingly.
6. **Edge deletion maintenance**: For a removed edge (u,v), the code identifies branches containing both u and v, derives updated maximal cliques after the deletion, and prunes/updates the prefix tree.
7. **Weighted clique computation**: By mapping edge weights to vertex weights, the framework supports both maximum vertex-weighted clique and maximum edge-weighted clique in a unified manner.
