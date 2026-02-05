#!/usr/bin/env python3
"""
简单的补丁脚本，用于在 llama-cli 中添加计算图导出功能
"""

patch_content = """
--- a/examples/main/main.cpp
+++ b/examples/main/main.cpp
@@ -100,6 +100,10 @@ int main(int argc, char ** argv) {
     params.sparams.n_keep = params.n_keep;
     params.sparams.seed = params.seed;
 
+    // 添加计算图导出选项
+    bool export_ops = false;
+    std::string export_ops_file = "operators.txt";
+
     // parse command line arguments
     if (!gpt_params_parse(argc, argv, params, LLAMA_EXAMPLE_MAIN, print_usage)) {
         return 1;
@@ -200,6 +204,16 @@ int main(int argc, char ** argv) {
         return 1;
     }
 
+    // 如果需要导出算子列表
+    if (export_ops) {
+        // 注意：这需要访问内部的 ggml_cgraph
+        // 实际使用中需要通过 llama_context 获取计算图
+        fprintf(stderr, "\\n");
+        fprintf(stderr, "To export operators, add ggml_graph_export_ops() call after llama_decode()\\n");
+        fprintf(stderr, "Example: ggml_graph_export_ops(lctx.gf, \\"%s\\");\\n", export_ops_file.c_str());
+        fprintf(stderr, "\\n");
+    }
+
     return 0;
 }
"""

print("补丁内容已生成。")
print("\n由于 llama-cli 的内部结构较复杂，推荐以下方式使用新功能：\n")
print("1. 直接修改 llama.cpp 内部代码")
print("2. 使用调试模式运行并添加日志")
print("3. 创建独立的工具程序\n")
