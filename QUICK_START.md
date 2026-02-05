# 快速使用指南

## 如何使用新添加的计算图导出功能

### 1. 快速测试 - 使用示例程序

最简单的方法是运行已经编译好的示例程序：

```bash
cd /home/klzhang/working/github/windstamp/llama.cpp/build
./bin/graph-analyzer my_operators.txt
```

这会创建一个简单的计算图并导出算子列表到 `my_operators.txt`。

### 2. 在实际模型中使用

如果你想在实际的 llama.cpp 推理过程中导出算子列表，需要修改 `llama.cpp` 源码：

#### 步骤1: 找到计算图构建的位置

在 `src/llama.cpp` 中，找到 `llama_decode` 函数或类似的地方，在计算图构建后添加：

```cpp
// 在计算图构建完成后
ggml_graph_export_ops(gf, "model_operators.txt");
ggml_graph_print_detailed(gf);
```

#### 步骤2: 添加导出接口（可选）

如果想要更优雅的方式，可以在 `include/llama.h` 中添加：

```cpp
// 导出当前模型的算子列表
LLAMA_API void llama_export_operators(
    struct llama_context * ctx,
    const char * filename
);
```

然后在 `src/llama.cpp` 中实现：

```cpp
void llama_export_operators(struct llama_context * ctx, const char * filename) {
    if (ctx && ctx->gf) {
        ggml_graph_export_ops(ctx->gf, filename);
    }
}
```

#### 步骤3: 在 llama-cli 中使用

修改 `examples/main/main.cpp`，添加命令行选项：

```cpp
// 在参数解析部分
if (params.export_ops) {
    llama_export_operators(ctx, "operators.txt");
}
```

### 3. 直接在你的代码中使用

如果你正在编写自己的程序使用 llama.cpp，可以直接调用 API：

```cpp
#include "ggml.h"
#include "llama.h"

int main() {
    // 初始化和构建模型...
    
    // 获取计算图（需要访问内部结构）
    struct ggml_cgraph * gf = /* 你的计算图 */;
    
    // 导出算子
    ggml_graph_export_ops(gf, "operators.txt");
    
    // 打印详细信息
    ggml_graph_print_detailed(gf);
    
    return 0;
}
```

### 4. 查看现有模型的算子

如果你只是想看看某个模型使用了哪些算子，推荐的快速方法：

1. 在 `llama_decode` 函数开始处添加静态标志：
   
```cpp
static bool ops_exported = false;
if (!ops_exported && gf) {
    ggml_graph_export_ops(gf, "model_operators.txt");
    ops_exported = true;
}
```

2. 重新编译：

```bash
cd build
make -j8
```

3. 运行任何推理命令：

```bash
./bin/llama-cli -m model.gguf -p "Hello" -n 1
```

4. 检查生成的 `model_operators.txt` 文件

### 5. 与现有日志结合

如果你启用了 GGML 的日志功能，`ggml_graph_print_detailed` 的输出会自动包含在日志中。

### 输出示例

执行后，你会得到类似这样的文件：

```
Found 15 unique operator types:
--------------------------------------------------------------------------------
ADD
CONT
DUP
GET_ROWS
GELU
MUL
MUL_MAT
PERMUTE
RESHAPE
RMS_NORM
ROPE
SCALE
SILU
SOFT_MAX
VIEW
--------------------------------------------------------------------------------
```

这个列表告诉你模型使用了哪些算子，可以用于：
- 检查硬件兼容性
- 性能分析  
- 算子优化决策
- 文档生成

### 常见问题

Q: 为什么我的输出文件是空的？
A: 确保在计算图**构建完成**后调用函数，而不是在构建之前。

Q: 如何包含所有层的算子？
A: 确保你在完整的前向传播后导出，而不是只在第一层后。

Q: 能否导出算子的使用频率？
A: 当前版本只导出唯一的算子类型。如需频率统计，可以修改 `ggml_graph_export_ops` 函数添加计数器。

Q: 支持哪些格式？
A: 当前只支持文本格式。如需JSON或其他格式，可以扩展函数。
