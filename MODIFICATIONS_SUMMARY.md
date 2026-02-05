# 计算图和算子列表导出功能 - 修改总结

## 修改概述

为 llama.cpp 项目添加了导出计算图和算子列表的功能，可以帮助分析模型使用的算子类型。

## 修改的文件

### 1. ggml/include/ggml.h
添加了两个新的API函数声明：

```c
// export unique operator types from the graph to a file
GGML_API void ggml_graph_export_ops(const struct ggml_cgraph * cgraph, const char * filename);

// print detailed graph information including operator types
GGML_API void ggml_graph_print_detailed(const struct ggml_cgraph * cgraph);
```

### 2. ggml/src/ggml.c
实现了两个新函数：

#### `ggml_graph_print_detailed()`
打印计算图的详细信息，包括：
- 节点名称
- 算子类型（包括 UNARY 子类型）
- 张量形状
- 数据类型

#### `ggml_graph_export_ops()`
导出所有唯一的算子类型到文件，特性：
- 自动去重
- 按字母顺序排序
- 支持 UNARY 操作的具体子类型（如 SILU, GELU）
- 输出格式清晰易读

### 3. 新增示例程序

#### examples/graph-analyzer/
完整的独立示例程序，演示如何使用新API：
- `graph-analyzer.c` - 主程序
- `CMakeLists.txt` - 编译配置  
- `README.md` - 使用说明

### 4. examples/CMakeLists.txt
添加了 graph-analyzer 子目录到编译系统

### 5. 文档
- `GRAPH_EXPORT_GUIDE.md` - 完整的使用指南和文档

## 使用方法

### 方法1: 在代码中直接调用

```c
#include "ggml.h"

// 构建计算图后
struct ggml_cgraph * gf = ...;

// 打印详细信息到控制台
ggml_graph_print_detailed(gf);

// 导出算子列表到文件
ggml_graph_export_ops(gf, "operators.txt");
```

### 方法2: 使用示例程序

```bash
# 编译
cd /home/klzhang/working/github/windstamp/llama.cpp/build
cmake ..
make graph-analyzer

# 运行
./bin/graph-analyzer [output_file.txt]
```

## 输出示例

### 控制台输出（ggml_graph_print_detailed）
```
=== DETAILED GRAPH ===
n_nodes = 7
 - 0: name='add_result', op='ADD', shape=[512, 10, 1, 1], type=f32
 - 1: name='mul_result', op='MUL', shape=[512, 10, 1, 1], type=f32
 - 2: name='rms_norm_result', op='RMS_NORM', shape=[512, 10, 1, 1], type=f32
 ...
```

### 文件输出（ggml_graph_export_ops）
```
Found 4 unique operator types:
--------------------------------------------------------------------------------
ADD
MUL
RMS_NORM
SCALE
--------------------------------------------------------------------------------
```

## 编译和测试

```bash
cd /home/klzhang/working/github/windstamp/llama.cpp
cd build
cmake ..
cmake --build . --target graph-analyzer --config Release -j4
./bin/graph-analyzer operators.txt
```

## 特性

1. **自动去重**: 只统计唯一的算子类型
2. **字母排序**: 输出结果按字母顺序排列
3. **详细信息**: 包含节点名称、形状、数据类型
4. **UNARY 支持**: 能够识别 UNARY 操作的具体子类型（SILU, GELU, RELU 等）
5. **简单易用**: API 设计简洁，易于集成到现有代码

## 应用场景

1. **模型分析**: 了解模型使用了哪些算子
2. **性能优化**: 识别需要优化的算子
3. **硬件适配**: 检查硬件是否支持所有算子
4. **调试**: 检查计算图结构是否正确
5. **文档**: 为模型生成算子使用文档

## 后续可能的改进

1. 添加算子使用频率统计
2. 支持 JSON/XML 格式输出
3. 添加算子执行时间分析
4. 可视化计算图（与现有的 dump_dot 功能集成）
5. 支持过滤特定类型的算子

## 注意事项

1. 需要在计算图构建完成后调用
2. 输出文件会覆盖已存在的同名文件
3. 函数是线程安全的
4. 对大型计算图，排序可能需要一些时间（但通常很快）

## 兼容性

- ✅ 与现有 GGML API 完全兼容
- ✅ 不影响现有功能
- ✅ 编译时无警告（除了示例程序的一些可忽略的警告）
- ✅ 支持所有 GGML 后端（CPU, CUDA, Metal 等）
