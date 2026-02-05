// 独立的计算图分析工具
// 这个示例展示了如何直接使用 GGML API 构建和分析计算图

#include "ggml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// 简单的模型结构
struct simple_model {
    struct ggml_tensor * embedding;
    struct ggml_tensor * norm_weight;
    struct ggml_tensor * attn_q_weight;
    struct ggml_tensor * attn_k_weight;
    struct ggml_tensor * attn_v_weight;
    struct ggml_tensor * output_weight;
    
    struct ggml_context * ctx;
};

// 创建一个简单的模型
struct simple_model * create_model(int vocab_size, int n_embd, int n_ctx) {
    struct simple_model * model = (struct simple_model *)malloc(sizeof(struct simple_model));
    
    // 计算所需内存
    size_t ctx_size = 0;
    ctx_size += vocab_size * n_embd * ggml_type_size(GGML_TYPE_F32); // embedding
    ctx_size += n_embd * ggml_type_size(GGML_TYPE_F32);               // norm_weight
    ctx_size += n_embd * n_embd * ggml_type_size(GGML_TYPE_F32) * 3; // q,k,v weights
    ctx_size += n_embd * vocab_size * ggml_type_size(GGML_TYPE_F32); // output_weight
    ctx_size += 1024 * 1024 * 100; // 额外缓冲
    
    struct ggml_init_params params = {
        /*.mem_size   =*/ ctx_size,
        /*.mem_buffer =*/ NULL,
        /*.no_alloc   =*/ false,
    };
    
    model->ctx = ggml_init(params);
    
    // 创建张量
    model->embedding = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F32, n_embd, vocab_size);
    ggml_set_name(model->embedding, "token_embd.weight");
    
    model->norm_weight = ggml_new_tensor_1d(model->ctx, GGML_TYPE_F32, n_embd);
    ggml_set_name(model->norm_weight, "norm.weight");
    
    model->attn_q_weight = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F32, n_embd, n_embd);
    ggml_set_name(model->attn_q_weight, "attn_q.weight");
    
    model->attn_k_weight = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F32, n_embd, n_embd);
    ggml_set_name(model->attn_k_weight, "attn_k.weight");
    
    model->attn_v_weight = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F32, n_embd, n_embd);
    ggml_set_name(model->attn_v_weight, "attn_v.weight");
    
    model->output_weight = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F32, vocab_size, n_embd);
    ggml_set_name(model->output_weight, "output.weight");
    
    return model;
}

// 构建前向计算图 - 最简化版本，展示各种算子类型
struct ggml_cgraph * build_graph(struct simple_model * model, int n_tokens, int n_embd) {
    struct ggml_cgraph * gf = ggml_new_graph(model->ctx);
    
    // 创建两个简单的张量用于演示
    struct ggml_tensor * a = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F32, n_embd, n_tokens);
    ggml_set_name(a, "tensor_a");
    ggml_set_input(a);
    
    struct ggml_tensor * b = ggml_new_tensor_2d(model->ctx, GGML_TYPE_F32, n_embd, n_tokens);
    ggml_set_name(b, "tensor_b");
    ggml_set_input(b);
    
    // 1. ADD operation
    struct ggml_tensor * add_result = ggml_add(model->ctx, a, b);
    ggml_set_name(add_result, "add_result");
    
    // 2. MUL operation
    struct ggml_tensor * mul_result = ggml_mul(model->ctx, a, b);
    ggml_set_name(mul_result, "mul_result");
    
    // 3. RMS_NORM operation
    struct ggml_tensor * norm_result = ggml_rms_norm(model->ctx, a, 1e-5f);
    ggml_set_name(norm_result, "rms_norm_result");
    
    // 4. SCALE operation
    struct ggml_tensor * scale_result = ggml_scale(model->ctx, a, 0.5f);
    ggml_set_name(scale_result, "scale_result");
    
    // 5. MUL_MAT operation 
    struct ggml_tensor * mat_result = ggml_mul_mat(model->ctx, model->attn_q_weight, a);
    ggml_set_name(mat_result, "mul_mat_result");
    
    // 6. TRANSPOSE operation
    struct ggml_tensor * trans_result = ggml_transpose(model->ctx, a);
    ggml_set_name(trans_result, "transpose_result");
    
    // 7. CONT operation
    struct ggml_tensor * cont_result = ggml_cont(model->ctx, trans_result);
    ggml_set_name(cont_result, "cont_result");
    
    // 8. RESHAPE operation
    struct ggml_tensor * reshape_result = ggml_reshape_3d(model->ctx, a, n_embd/4, 4, n_tokens);
    ggml_set_name(reshape_result, "reshape_result");
    
    // 9. PERMUTE operation
    struct ggml_tensor * permute_result = ggml_permute(model->ctx, reshape_result, 0, 2, 1, 3);
    ggml_set_name(permute_result, "permute_result");
    
    // 10. VIEW operation
    struct ggml_tensor * view_result = ggml_view_2d(model->ctx, a, n_embd/2, n_tokens, a->nb[1], 0);
    ggml_set_name(view_result, "view_result");
    
    // 11. SOFT_MAX operation
    struct ggml_tensor * softmax_result = ggml_soft_max(model->ctx, a);
    ggml_set_name(softmax_result, "softmax_result");
    
    // 12. GET_ROWS operation (embedding lookup)
    struct ggml_tensor * inp_tokens = ggml_new_tensor_1d(model->ctx, GGML_TYPE_I32, n_tokens);
    ggml_set_name(inp_tokens, "inp_tokens");
    ggml_set_input(inp_tokens);
    struct ggml_tensor * embd_result = ggml_get_rows(model->ctx, model->embedding, inp_tokens);
    ggml_set_name(embd_result, "get_rows_result");
    
    // Combine some results to create final output
    struct ggml_tensor * combined = ggml_add(model->ctx, add_result, mul_result);
    combined = ggml_add(model->ctx, combined, norm_result);
    combined = ggml_add(model->ctx, combined, scale_result);
    ggml_set_name(combined, "final_output");
    
    // 构建计算图
    ggml_build_forward_expand(gf, combined);
    
    return gf;
}

void free_model(struct simple_model * model) {
    if (model) {
        if (model->ctx) {
            ggml_free(model->ctx);
        }
        free(model);
    }
}

int main(int argc, char ** argv) {
    // 参数
    const int vocab_size = 32000;
    const int n_embd = 512;
    const int n_ctx = 128;
    const int n_tokens = 10;
    
    const char * output_file = "operators.txt";
    
    if (argc > 1) {
        output_file = argv[1];
    }
    
    printf("Creating simple transformer model...\n");
    printf("  vocab_size = %d\n", vocab_size);
    printf("  n_embd = %d\n", n_embd);
    printf("  n_ctx = %d\n", n_ctx);
    printf("  n_tokens = %d\n", n_tokens);
    printf("\n");
    
    // 创建模型
    struct simple_model * model = create_model(vocab_size, n_embd, n_ctx);
    if (!model || !model->ctx) {
        fprintf(stderr, "Failed to create model\n");
        return 1;
    }
    
    // 构建计算图
    printf("Building computation graph...\n");
    struct ggml_cgraph * gf = build_graph(model, n_tokens, n_embd);
    
    // 打印基本信息
    printf("\n");
    ggml_graph_print(gf);
    
    // 打印详细信息
    printf("\n");
    ggml_graph_print_detailed(gf);
    
    // 导出算子列表
    printf("\nExporting operator list to '%s'...\n", output_file);
    ggml_graph_export_ops(gf, output_file);
    
    printf("\nDone!\n");
    printf("Check '%s' for the list of unique operators.\n", output_file);
    
    // 清理
    free_model(model);
    
    return 0;
}
