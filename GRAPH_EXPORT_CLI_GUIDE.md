# 计算图导出功能使用指南 / Graph Export Feature Guide

## 中文说明

### 功能介绍

llama-cli 现在支持在模型推理过程中自动导出计算图的算子列表。这个功能可以帮助您分析模型使用了哪些 GGML 算子。

### 使用方法

在 llama-cli 命令中添加 `--export-ops` 参数：

```bash
./llama-cli -m <模型路径> -p "提示词" --export-ops operators.txt
```

### 示例

```bash
# 基本使用
./llama-cli -m DeepSeek-R1-Distill-Qwen-1.5B-f32.gguf -p "Hello AI" --export-ops operators.txt

# 完整示例（带更多参数）
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

### 输出示例

生成的 `operators.txt` 文件内容如下：

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

### 导出时机

- 算子列表会在第一次成功执行 `llama_decode()` 之后导出
- 只会导出一次，不会重复导出
- 导出时会在控制台输出提示信息

### 注意事项

1. 导出的算子列表是去重并按字母顺序排序的
2. 导出文件默认名称为 `operators.txt`，可以通过参数自定义
3. 如果文件已存在，会被覆盖

---

## English Guide

### Feature Introduction

llama-cli now supports automatic export of computation graph operators during model inference. This feature helps you analyze which GGML operators are used by the model.

### Usage

Add the `--export-ops` parameter to your llama-cli command:

```bash
./llama-cli -m <model_path> -p "prompt" --export-ops operators.txt
```

### Examples

```bash
# Basic usage
./llama-cli -m DeepSeek-R1-Distill-Qwen-1.5B-f32.gguf -p "Hello AI" --export-ops operators.txt

# Full example (with more parameters)
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

### Output Example

The generated `operators.txt` file will look like this:

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

### Export Timing

- The operator list is exported after the first successful `llama_decode()` execution
- Exported only once, no duplicates
- A confirmation message will be printed to the console

### Notes

1. The exported operator list is deduplicated and sorted alphabetically
2. Default export file name is `operators.txt`, but can be customized via parameter
3. If the file already exists, it will be overwritten

---

## Technical Details / 技术细节

### Modified Files / 修改的文件

1. **common/common.h** - Added `export_graph_ops` and `export_graph_ops_file` parameters
2. **common/arg.cpp** - Added `--export-ops` command-line argument parsing
3. **tools/main/main.cpp** - Added export logic after `llama_decode()`
4. **src/llama-context.cpp** - Implemented `llama_export_graph_ops()` function

### API Functions / API 函数

- `llama_export_graph_ops(ctx, filename)` - Export operator list to file
- `llama_print_graph_detailed(ctx)` - Print detailed graph information to console

### Integration / 集成

The export functionality integrates seamlessly with existing llama-cli workflow:
1. Model loads normally
2. First inference step executes
3. Computation graph is captured
4. Operators are extracted and exported
5. Inference continues as usual

导出功能与现有的 llama-cli 工作流程无缝集成：
1. 模型正常加载
2. 执行第一步推理
3. 捕获计算图
4. 提取并导出算子
5. 推理继续正常进行
