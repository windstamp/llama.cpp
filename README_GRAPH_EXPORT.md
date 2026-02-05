# 计算图和算子列表导出功能 - README

## 快速链接

- 🇨🇳 [中文说明](修改说明_中文.md)
- 📖 [详细文档](GRAPH_EXPORT_GUIDE.md)
- 🚀 [快速开始](QUICK_START.md)
- 📝 [修改总结](MODIFICATIONS_SUMMARY.md)
- 📋 [变更清单](CHANGELOG.md)

## 一句话总结

为 llama.cpp 添加了两个新 API，可以导出计算图中的算子列表到文件，并打印详细的图结构信息。

## 新增 API

```c
// 1. 导出唯一的算子类型到文件（按字母排序）
void ggml_graph_export_ops(const struct ggml_cgraph * cgraph, const char * filename);

// 2. 打印计算图的详细信息
void ggml_graph_print_detailed(const struct ggml_cgraph * cgraph);
```

## 30秒快速测试

```bash
cd /home/klzhang/working/github/windstamp/llama.cpp/build
./bin/graph-analyzer my_ops.txt
cat my_ops.txt
```

输出示例:
```
Found 4 unique operator types:
--------------------------------------------------------------------------------
ADD
MUL
RMS_NORM
SCALE
--------------------------------------------------------------------------------
```

## 核心文件

| 文件 | 说明 |
|------|------|
| [ggml/include/ggml.h](ggml/include/ggml.h) | API 声明 |
| [ggml/src/ggml.c](ggml/src/ggml.c) | API 实现 |
| [examples/graph-analyzer/](examples/graph-analyzer/) | 示例程序 |

## 使用场景

1. **模型分析** - 快速了解模型使用的算子类型
2. **性能优化** - 识别需要优化的算子
3. **硬件适配** - 检查目标硬件是否支持所有算子
4. **调试** - 验证计算图结构
5. **文档** - 自动生成算子列表文档

## 主要特性

- ✅ 自动去重算子类型
- ✅ 按字母顺序排序
- ✅ 支持 UNARY 子类型（SILU, GELU 等）
- ✅ 输出格式清晰易读
- ✅ 线程安全
- ✅ 零性能开销（仅在调用时执行）

## 编译

```bash
cd build
cmake ..
make graph-analyzer
```

## 集成到你的代码

```c
#include "ggml.h"

// 在计算图构建完成后
ggml_graph_export_ops(your_graph, "operators.txt");
```

## 文档结构

```
.
├── 修改说明_中文.md          # 中文完整说明（推荐首先阅读）
├── MODIFICATIONS_SUMMARY.md  # 英文修改总结
├── GRAPH_EXPORT_GUIDE.md     # 详细使用指南
├── QUICK_START.md            # 快速开始指南
├── CHANGELOG.md              # 详细变更清单
└── README_GRAPH_EXPORT.md    # 本文件
```

## 示例输出

### 文件输出
```
Found 8 unique operator types:
--------------------------------------------------------------------------------
ADD
CONT
GET_ROWS
MUL_MAT
PERMUTE
RMS_NORM
SCALE
SOFT_MAX
--------------------------------------------------------------------------------
```

### 控制台输出
```
=== DETAILED GRAPH ===
n_nodes = 7
 - 0: name='add_result', op='ADD', shape=[512, 10, 1, 1], type=f32
 - 1: name='mul_result', op='MUL', shape=[512, 10, 1, 1], type=f32
 - 2: name='norm_result', op='RMS_NORM', shape=[512, 10, 1, 1], type=f32
...
```

## Git 提交

```bash
# 添加所有修改
git add ggml/include/ggml.h ggml/src/ggml.c
git add examples/CMakeLists.txt examples/graph-analyzer/
git add *.md

# 提交
git commit -m "feat: Add computation graph and operator list export

- Add ggml_graph_export_ops() and ggml_graph_print_detailed()
- Add graph-analyzer example program
- Add comprehensive documentation"
```

## 支持和反馈

如有问题，请查阅:
1. [快速开始指南](QUICK_START.md) - 常见问题解答
2. [详细文档](GRAPH_EXPORT_GUIDE.md) - 深入使用方法
3. [中文说明](修改说明_中文.md) - 完整中文文档

## 许可证

与 llama.cpp 主项目保持一致 (MIT License)

---

**作者**: zkl  
**日期**: 2026-02-03  
**版本**: 1.0.0
