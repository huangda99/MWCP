Build
In the project root directory (the directory containing MakeFile), run:

bash
make
After successful compilation, the executable file will be generated:

bIndex.out

To clean:

bash
make clean
Usage
The unified entry point for the executable is:

bash
./bIndex.out <graph_file_path> <add|delete|none> <edge_update_file_path>
Parameter Description:

<graph_file_path>: Path to the graph data file.

<add|delete|none>: Maintenance mode

add: After building the maximal clique index, perform edge addition maintenance once.

delete: After building the maximal clique index, perform edge deletion maintenance once.

none: Only build the maximal clique index without any maintenance ( <edge_update_file_path> can be filled with any placeholder).

<edge_update_file_path>: Path to the edge addition/deletion data file.

Examples:

bash
# Only build the maximal clique index
./bIndex.out datasets/graph.txt none _

# Build the index, then perform edge addition maintenance based on edges in add_edges.txt
./bIndex.out datasets/graph.txt add datasets/add_edges.txt

# Build the index, then perform edge deletion maintenance based on edges in delete_edges.txt
./bIndex.out datasets/graph.txt delete datasets/delete_edges.txt
During execution, the program outputs:

Statistics such as maximal clique index construction time and degeneracy.

Memory usage information for the index (prefix tree).

When selecting add / delete, the runtime of the corresponding maintenance process and memory information after maintenance.
