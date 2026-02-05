# llama.cpp 计算图和算子列表导出功能

## 📋 修改说明

本次修改为 llama.cpp 添加了两个新的 API 函数，用于**导出计算图信息和算子列表**。

## 🎯 主要功能

### 1. `ggml_graph_export_ops()` - 导出算子列表
将计算图中所有唯一的算子类型导出到文件，按字母顺序排序。

### 2. `ggml_graph_print_detailed()` - 打印详细信息
打印计算图的详细信息到控制台，包括每个节点的名称、算子类型、形状和数据类型。

## 📝 修改的文件

| 文件 | 修改内容 |
|------|---------|
| `ggml/include/ggml.h` | 添加了 2 个新 API 函数声明 |
| `ggml/src/ggml.c` | 实现了 2 个新函数（共约 120 行代码）|
| `examples/CMakeLists.txt` | 添加 graph-analyzer 示例 |
| `examples/graph-analyzer/` | 新增完整的示例程序 |

## 🚀 编译步骤

```bash
cd /home/klzhang/working/github/windstamp/llama.cpp
cd build
cmake ..
cmake --build . --target graph-analyzer --config Release -j8
```

## 💡 使用示例

### 运行示例程序
```bash
./bin/graph-analyzer operators.txt
```

### 在代码中使用
```c
// 方法1: 导出到文件
ggml_graph_export_ops(cgraph, "operators.txt");

// 方法2: 打印详细信息
ggml_graph_print_detailed(cgraph);
```

## 📄 输出格式

### 文件输出示例 (operators.txt)
```
Found 4 unique operator types:
--------------------------------------------------------------------------------
ADD
MUL
RMS_NORM
SCALE
--------------------------------------------------------------------------------
```

### 控制台输出示例
```
=== DETAILED GRAPH ===
n_nodes = 7
 - 0: name='add_result', op='ADD', shape=[512, 10, 1, 1], type=f32
 - 1: name='mul_result', op='MUL', shape=[512, 10, 1, 1], type=f32
 - 2: name='rms_norm_result', op='RMS_NORM', shape=[512, 10, 1, 1], type=f32
...
```

## ✅ 测试结果

已成功编译和测试：
- ✅ 编译通过，无错误
- ✅ 示例程序运行正常
- ✅ 算子列表导出成功
- ✅ 详细信息打印正常
- ✅ 算子去重和排序正确

## 🔧 技术细节

### 实现特点
1. **自动去重**: 使用布尔数组追踪已见算子
2. **字母排序**: 冒泡排序实现（对于小规模数据足够高效）
3. **UNARY 支持**: 特殊处理 UNARY 操作，显示具体子类型（如 SILU, GELU）
4. **错误处理**: 文件打开失败时输出错误信息

### 代码结构
```
ggml_graph_export_ops()
├── 扫描所有节点
├── 收集唯一算子类型
├── 排序算子名称
└── 写入文件

ggml_graph_print_detailed()
├── 打印节点信息（包含名称、算子、形状、类型）
└── 打印叶子节点信息
```

## 📚 相关文档

- [MODIFICATIONS_SUMMARY.md](MODIFICATIONS_SUMMARY.md) - 详细修改总结
- [GRAPH_EXPORT_GUIDE.md](GRAPH_EXPORT_GUIDE.md) - 完整使用指南
- [QUICK_START.md](QUICK_START.md) - 快速开始指南
- [examples/graph-analyzer/README.md](examples/graph-analyzer/README.md) - 示例程序说明

## 🎨 应用场景

1. **模型分析** - 了解模型使用的算子类型
2. **性能优化** - 识别瓶颈算子
3. **硬件适配** - 检查算子兼容性
4. **调试工具** - 验证计算图结构
5. **文档生成** - 自动生成算子列表文档

## 🔮 未来改进方向

- [ ] 添加算子使用频率统计
- [ ] 支持 JSON 格式输出
- [ ] 添加算子耗时分析
- [ ] 可视化计算图
- [ ] 支持算子过滤

## 📞 使用方式

### 方式1: 直接使用示例程序（推荐用于快速测试）
```bash
cd build
./bin/graph-analyzer my_output.txt
```

### 方式2: 集成到现有代码（推荐用于生产环境）
在你的 C/C++ 代码中：
```c
#include "ggml.h"

// 在计算图构建完成后
ggml_graph_export_ops(gf, "operators.txt");
ggml_graph_print_detailed(gf);
```

### 方式3: 修改 llama-cli（用于分析实际模型）
在 `llama_decode` 函数中添加：
```cpp
static bool exported = false;
if (!exported) {
    ggml_graph_export_ops(lctx.gf, "model_operators.txt");
    exported = true;
}
```

## ⚠️ 注意事项

1. 必须在计算图构建**完成后**调用
2. 输出文件会**覆盖**同名文件
3. 大型计算图可能需要稍多内存（但通常可忽略）
4. 线程安全，可在多线程环境使用

## 🎯 git 提交建议

```bash
git add ggml/include/ggml.h ggml/src/ggml.c
git add examples/CMakeLists.txt
git add examples/graph-analyzer/
git add *.md

git commit -m "feat: Add computation graph and operator list export功能

- Add ggml_graph_export_ops() to export unique operator types
- Add ggml_graph_print_detailed() for detailed graph information  
- Add graph-analyzer example program
- Support automatic deduplication and alphabetical sorting
- Handle UNARY operator subtypes correctly"
```

## 📊 代码统计

- 新增函数: 2 个
- 新增代码行数: ~120 行
- 示例程序: ~180 行
- 文档: ~500 行
- 修改的文件: 4 个
- 新增的文件: 6 个
