# llama-cli 计算图导出功能实现总结

## 实现目标

在执行 `llama-cli -m ...` 命令时，能够自动导出计算图和算子列表到文件。

## 完成的修改

### 1. 添加命令行参数支持 (common/common.h)

在 `common_params` 结构体中添加了两个新字段：

```cpp
bool export_graph_ops  = false;                      // 是否导出计算图算子
std::string export_graph_ops_file = "operators.txt"; // 导出文件路径
```

### 2. 实现参数解析 (common/arg.cpp)

添加了 `--export-ops` 命令行参数的解析逻辑：

```cpp
add_opt(common_arg(
    {"--export-ops"}, "FNAME",
    string_format("export computation graph operators to file (default: %s)", 
                  params.export_graph_ops_file.c_str()),
    [](common_params & params, const std::string & value) {
        params.export_graph_ops = true;
        params.export_graph_ops_file = value;
    }
));
```

### 3. 集成到 llama-cli (tools/main/main.cpp)

**添加状态跟踪变量：**

```cpp
bool graph_exported = false; // 跟踪是否已导出计算图
```

**在 llama_decode() 后添加导出逻辑：**

```cpp
if (llama_decode(ctx, llama_batch_get_one(&embd[i], n_eval))) {
    LOG_ERR("%s : failed to eval\n", __func__);
    return 1;
}

// 在第一次成功 decode 后导出计算图算子
if (params.export_graph_ops && !graph_exported) {
    llama_export_graph_ops(ctx, params.export_graph_ops_file.c_str());
    LOG_INF("Exported computation graph operators to: %s\n", 
            params.export_graph_ops_file.c_str());
    graph_exported = true;
}
```

### 4. 实现导出函数 (src/llama-context.cpp)

```cpp
void llama_export_graph_ops(struct llama_context * ctx, const char * filename) {
    if (!ctx || !filename) {
        return;
    }
    
    // 从最近的评估中获取计算图
    auto * gf_res = ctx->get_gf_res_reserve();
    if (gf_res) {
        auto * gf = gf_res->get_gf();
        if (gf) {
            ggml_graph_export_ops(gf, filename);
        }
    }
}
```

## 使用方法

```bash
# 基本用法
./llama-cli -m model.gguf -p "Hello" --export-ops operators.txt

# 完整示例
./llama-cli -m DeepSeek-R1-Distill-Qwen-1.5B-f32.gguf \
    -p "Hello AI" \
    --color \
    -n 50 \
    --seed 2025 \
    --temp 0 \
    -t 1 \
    -ngl 0 \
    -b 1 \
    --export-ops operators.txt
```

## 测试结果

使用 DeepSeek-R1-Distill-Qwen-1.5B-f32 模型测试，成功导出了 13 种算子类型：

```
Found 13 unique operator types:
--------------------------------------------------------------------------------
ADD
CPY
FLASH_ATTN_EXT
GET_ROWS
GLU
MUL
MUL_MAT
PERMUTE
RESHAPE
RMS_NORM
ROPE
SET_ROWS
VIEW
--------------------------------------------------------------------------------
```

## 技术特点

1. **无侵入性**：不影响原有的推理流程
2. **高效性**：只在第一次 decode 后导出一次
3. **易用性**：单个命令行参数即可启用
4. **准确性**：直接从实际推理的计算图中提取算子
5. **可扩展性**：基础 API 可用于其他工具和用途

## 依赖的底层 API

- `ggml_graph_export_ops()` - GGML 层面的算子导出函数
- `ggml_graph_print_detailed()` - GGML 层面的图打印函数
- 这些函数已在之前的修改中实现

## 文件树

```
llama.cpp/
├── include/llama.h                    # API 声明
├── src/llama-context.cpp              # 导出函数实现
├── common/
│   ├── common.h                       # 参数结构定义
│   └── arg.cpp                        # 参数解析
├── tools/main/
│   └── main.cpp                       # llama-cli 集成
├── ggml/
│   ├── include/ggml.h                 # GGML API 声明
│   └── src/ggml.c                     # GGML 实现
└── GRAPH_EXPORT_CLI_GUIDE.md          # 使用指南
```

## 编译

```bash
cd build
make llama-cli -j8
```

编译成功，所有修改都通过了编译检查。

## 后续可能的改进

1. 添加 JSON 格式的导出选项
2. 支持导出更详细的算子信息（维度、参数等）
3. 支持导出完整的计算图结构（节点和边）
4. 添加统计信息（算子数量、内存使用等）

## 总结

成功实现了在 llama-cli 执行过程中自动导出计算图算子列表的功能。用户只需添加一个命令行参数 `--export-ops <文件名>` 即可使用。该功能已经过测试，能够正确导出实际推理中使用的所有算子类型。
