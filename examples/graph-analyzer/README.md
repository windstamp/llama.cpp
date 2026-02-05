# Graph Analyzer

一个简单的工具，用于演示如何使用 GGML 的计算图导出功能。

## 编译

```bash
cd /home/klzhang/working/github/windstamp/llama.cpp
mkdir -p build
cd build
cmake ..
make graph-analyzer
```

## 运行

```bash
# 使用默认输出文件 (operators.txt)
./bin/graph-analyzer

# 指定输出文件
./bin/graph-analyzer my_operators.txt
```

## 输出示例

程序会：
1. 创建一个简单的 Transformer 模型
2. 构建前向计算图
3. 打印计算图的基本信息
4. 打印计算图的详细信息
5. 导出唯一的算子类型列表到文件

输出文件内容示例：
```
Found 8 unique operator types:
--------------------------------------------------------------------------------
ADD
GET_ROWS
MUL
MUL_MAT
RMS_NORM
SCALE
SOFT_MAX
--------------------------------------------------------------------------------
```
