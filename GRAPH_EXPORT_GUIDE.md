# 计算图和算子列表导出功能

## 概述

本修改为 llama.cpp 添加了导出计算图和算子列表的功能。

## 新增API

### 1. `ggml_graph_export_ops`

导出计算图中所有唯一的算子类型到文件（按字母顺序排序）。

```c
void ggml_graph_export_ops(const struct ggml_cgraph * cgraph, const char * filename);
```

**参数：**
- `cgraph`: 计算图指针
- `filename`: 输出文件路径

**示例：**
```c
ggml_graph_export_ops(gf, "operators.txt");
```

### 2. `ggml_graph_print_detailed`

打印计算图的详细信息，包括每个节点的名称、算子类型、形状和数据类型。

```c
void ggml_graph_print_detailed(const struct ggml_cgraph * cgraph);
```

**参数：**
- `cgraph`: 计算图指针

**示例：**
```c
ggml_graph_print_detailed(gf);
```

## 修改的文件

1. **ggml/include/ggml.h**
   - 添加了 `ggml_graph_export_ops` 函数声明
   - 添加了 `ggml_graph_print_detailed` 函数声明

2. **ggml/src/ggml.c**
   - 实现了 `ggml_graph_export_ops` 函数
   - 实现了 `ggml_graph_print_detailed` 函数

3. **examples/export-graph/** (新增)
   - 提供了示例程序展示如何使用新API

## 使用方法

### 方法1: 在现有代码中使用

在你的代码中，当构建完计算图后，可以调用这些函数：

```c
// 在 llama.cpp 或其他使用 ggml 的代码中
struct ggml_cgraph * gf = ggml_new_graph(ctx);
ggml_build_forward_expand(gf, output_tensor);

// 导出算子列表到文件
ggml_graph_export_ops(gf, "operators.txt");

// 打印详细的计算图信息
ggml_graph_print_detailed(gf);
```

### 方法2: 修改 llama-cli

在 `examples/main/main.cpp` 中添加导出功能：

```cpp
// 在推理循环之前或之后添加
if (params.export_graph) {
    // 需要访问内部的计算图
    // 这需要在 llama.cpp 中添加相应的接口函数
}
```

### 方法3: 使用示例程序

```bash
# 编译
cd build
cmake .. -DGGML_CUDA=ON  # 或其他选项
make export-graph

# 运行
./bin/export-graph -m /path/to/model.gguf -p "Hello" -o operators.txt
```

## 输出格式

### operators.txt 示例

```
Found 15 unique operator types:
--------------------------------------------------------------------------------
ADD
CONT
DUP
GET_ROWS
MUL
MUL_MAT
NORM
PERMUTE
RESHAPE
RMS_NORM
ROPE
SCALE
SOFT_MAX
VIEW
--------------------------------------------------------------------------------
```

### 详细输出示例

```
=== DETAILED GRAPH ===
n_nodes = 120
 - 0: name='inp_tokens', op='GET_ROWS', shape=[512, 4096, 1, 1], type=f32
 - 1: name='layer_0_attn_norm', op='RMS_NORM', shape=[512, 4096, 1, 1], type=f32
 - 2: name='layer_0_attn_q', op='MUL_MAT', shape=[512, 4096, 1, 1], type=f32
 ...
```

## 编译和安装

重新编译项目：

```bash
cd /home/klzhang/working/github/windstamp/llama.cpp
mkdir -p build
cd build
cmake .. -DGGML_CUDA=ON  # 根据需要选择后端
cmake --build . --config Release -j8
```

## 高级用法：暴露计算图

如果需要从 llama-cli 或其他工具直接访问计算图，可以在 `llama.cpp` 中添加：

```cpp
// 在 llama.cpp 中添加
struct ggml_cgraph * llama_get_compute_graph(struct llama_context * ctx) {
    return &ctx->gf;  // 或相应的计算图
}
```

然后在 `llama.h` 中声明：

```cpp
LLAMA_API struct ggml_cgraph * llama_get_compute_graph(struct llama_context * ctx);
```

## 应用场景

1. **性能分析**：了解模型使用了哪些算子
2. **算子优化**：针对特定算子进行优化
3. **硬件适配**：检查硬件是否支持所有算子
4. **模型调试**：检查计算图结构
5. **算子统计**：分析算子使用频率

## 注意事项

1. 这些函数需要在计算图构建完成后调用
2. 输出文件会覆盖已存在的同名文件
3. 对于 UNARY 操作，会显示具体的 unary 操作类型（如 SILU, GELU 等）
4. 算子列表按字母顺序排序，便于查看

## 后续改进建议

1. 添加算子使用频率统计
2. 支持 JSON 格式输出
3. 添加可视化工具（如生成 DOT 图）
4. 支持过滤特定类型的算子
5. 添加算子执行时间分析
