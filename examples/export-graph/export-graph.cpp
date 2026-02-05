#include "ggml.h"
#include "llama.h"
#include <cstdio>
#include <cstring>
#include <string>

static void print_usage(int argc, char ** argv, const struct llama_params & params) {
    fprintf(stderr, "usage: %s [options]\n", argv[0]);
    fprintf(stderr, "\n");
    fprintf(stderr, "options:\n");
    fprintf(stderr, "  -h, --help              show this help message and exit\n");
    fprintf(stderr, "  -m, --model FNAME       model path (default: %s)\n", params.model.c_str());
    fprintf(stderr, "  -p, --prompt PROMPT     prompt to use (default: empty)\n");
    fprintf(stderr, "  -o, --output FNAME      output file for operator list (default: operators.txt)\n");
    fprintf(stderr, "  -n, --n-predict N       number of tokens to predict (default: %d)\n", params.n_predict);
    fprintf(stderr, "\n");
}

int main(int argc, char ** argv) {
    llama_params params;
    params.n_predict = 32;
    params.prompt = "Hello";
    
    std::string output_file = "operators.txt";

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            print_usage(argc, argv, params);
            return 0;
        } else if (arg == "-m" || arg == "--model") {
            if (i + 1 < argc) {
                params.model = argv[++i];
            } else {
                fprintf(stderr, "error: missing model path\n");
                return 1;
            }
        } else if (arg == "-p" || arg == "--prompt") {
            if (i + 1 < argc) {
                params.prompt = argv[++i];
            } else {
                fprintf(stderr, "error: missing prompt\n");
                return 1;
            }
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                output_file = argv[++i];
            } else {
                fprintf(stderr, "error: missing output file\n");
                return 1;
            }
        } else if (arg == "-n" || arg == "--n-predict") {
            if (i + 1 < argc) {
                params.n_predict = atoi(argv[++i]);
            } else {
                fprintf(stderr, "error: missing n-predict value\n");
                return 1;
            }
        } else {
            fprintf(stderr, "error: unknown argument: %s\n", arg.c_str());
            print_usage(argc, argv, params);
            return 1;
        }
    }

    // Check if model path is provided
    if (params.model.empty()) {
        fprintf(stderr, "error: no model path provided\n");
        print_usage(argc, argv, params);
        return 1;
    }

    // Initialize llama backend
    llama_backend_init();
    llama_numa_init(GGML_NUMA_STRATEGY_DISABLED);

    // Load model
    fprintf(stderr, "Loading model from %s ...\n", params.model.c_str());
    llama_model_params model_params = llama_model_default_params();
    llama_model * model = llama_load_model_from_file(params.model.c_str(), model_params);
    
    if (model == NULL) {
        fprintf(stderr, "error: failed to load model\n");
        return 1;
    }

    // Create context
    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = 512;
    ctx_params.n_batch = 512;
    ctx_params.n_threads = 4;
    ctx_params.n_threads_batch = 4;
    
    llama_context * ctx = llama_new_context_with_model(model, ctx_params);
    
    if (ctx == NULL) {
        fprintf(stderr, "error: failed to create context\n");
        llama_free_model(model);
        return 1;
    }

    // Tokenize prompt
    std::vector<llama_token> tokens;
    tokens.resize(params.prompt.size() + 1);
    int n_tokens = llama_tokenize(model, params.prompt.c_str(), params.prompt.size(), 
                                   tokens.data(), tokens.size(), true, false);
    
    if (n_tokens < 0) {
        tokens.resize(-n_tokens);
        n_tokens = llama_tokenize(model, params.prompt.c_str(), params.prompt.size(), 
                                  tokens.data(), tokens.size(), true, false);
    }
    tokens.resize(n_tokens);

    fprintf(stderr, "Prompt tokens: %d\n", n_tokens);

    // Decode to build computation graph
    llama_batch batch = llama_batch_init(n_tokens, 0, 1);
    
    for (int i = 0; i < n_tokens; i++) {
        llama_batch_add(batch, tokens[i], i, {0}, false);
    }
    batch.logits[batch.n_tokens - 1] = true;

    fprintf(stderr, "Building computation graph...\n");
    if (llama_decode(ctx, batch) != 0) {
        fprintf(stderr, "error: failed to decode\n");
        llama_batch_free(batch);
        llama_free(ctx);
        llama_free_model(model);
        return 1;
    }

    // Get the computation graph
    // Note: This is a simplified example. In practice, you might need to access
    // the internal computation graph through llama's internal structures
    fprintf(stderr, "Note: Direct graph access is limited in the public API.\n");
    fprintf(stderr, "For full graph export, you may need to modify llama.cpp internals.\n");
    
    fprintf(stderr, "\nTo export the graph operators, you need to:\n");
    fprintf(stderr, "1. Add a function in llama.cpp that exposes the computation graph\n");
    fprintf(stderr, "2. Call ggml_graph_export_ops() or ggml_graph_print_detailed()\n");
    fprintf(stderr, "\nExample usage in code:\n");
    fprintf(stderr, "  ggml_graph_export_ops(cgraph, \"%s\");\n", output_file.c_str());
    fprintf(stderr, "  ggml_graph_print_detailed(cgraph);\n");

    // Cleanup
    llama_batch_free(batch);
    llama_free(ctx);
    llama_free_model(model);
    llama_backend_free();

    fprintf(stderr, "\nDone!\n");
    return 0;
}
