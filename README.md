## 编译

在项目根目录（包含 `MakeFile` 的目录）执行：

```bash
make
```

编译成功后会生成可执行文件：

- `bIndex.out`

如需清理：

```bash
make clean
```
## 运行方式

可执行文件统一入口为：

```bash
./bIndex.out <graph_file_path> <add|delete|none> <edge_update_file_path>
```

参数说明：
- `<graph_file_path>`：图数据文件路径。
- `<add|delete|none>`：维护模式
  - `add`：在构建极大团索引后，执行一次**加边维护**。
  - `delete`：在构建极大团索引后，执行一次**删边维护**。
  - `none`：仅构建极大团索引，不进行任何维护（`edge_update_file_path` 可填任意占位）。
- `<edge_update_file_path>`：加边 / 删边数据文件路径。

示例：

```bash
# 只构建极大团索引
./bIndex.out datasets/uai.txt none _

# 构建索引后，根据 add_edges.txt 中的边进行加边维护
./bIndex.out datasets/uai.txt add datasets/uai_add.txt

# 构建索引后，根据 delete_edges.txt 中的边进行删边维护
./bIndex.out datasets/uai.txt delete datasets/uai_delete.txt
```

程序运行过程中会输出：
- 极大团索引构建时间、退化度等统计信息。
- 索引（前缀树）占用的内存信息。
- 在选择 `add` / `delete` 时，对应维护过程的运行时间及维护后的内存信息。




