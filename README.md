Compilation

In the project root directory (the directory containing the MakeFile), run:

make


After successful compilation, the following executable will be generated:

bIndex.out

To clean the project:

make clean

Usage

The executable provides a unified entry point:

./bIndex.out <graph_file_path> <add|delete|none> <edge_update_file_path>

Parameters

<graph_file_path>: Path to the graph data file.

<add|delete|none>: Maintenance mode

add: After building the maximal clique index, perform one edge insertion maintenance.

delete: After building the maximal clique index, perform one edge deletion maintenance.

none: Only build the maximal clique index without performing any maintenance (edge_update_file_path can be any placeholder).

<edge_update_file_path>: Path to the edge insertion/deletion data file.

Examples
# Only build the maximal clique index
./bIndex.out datasets/graph.txt none _

# Build the index, then perform edge insertion maintenance using edges from add_edges.txt
./bIndex.out datasets/graph.txt add datasets/add_edges.txt

# Build the index, then perform edge deletion maintenance using edges from delete_edges.txt
./bIndex.out datasets/graph.txt delete datasets/delete_edges.txt

Output

During execution, the program outputs:

Statistics such as maximal clique index construction time and graph degeneracy.

Memory usage of the index (prefix tree).

When add or delete is selected, the runtime of the corresponding maintenance procedure and the memory usage after maintenance.
