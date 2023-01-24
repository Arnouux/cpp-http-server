#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "trie_t.h"


int find(const trie_node_t *root, const url_t *url) {
    const trie_node_t *current_node = root;
    int current_token_i = 0;

    int found = 1;
    while(found && current_token_i < url->size) {
        found = 0;
        for(int i=0; i<current_node->children_size; ++i) {
            if(!strcmp(current_node->children[i]->word,url->tokens[current_token_i])
                || !strcmp("*", current_node->children[i]->word)) {
                if(url->size == current_token_i+1 && current_node->children[i]->is_leaf) {
                    return 1;
                }
                current_token_i++;
                current_node = current_node->children[i];
                found = 1;
            }
            else if(!strcmp("**", current_node->children[i]->word)) {
                return 1;
            }
        }
    }
    return 0;
}

const url_t *tokenize_url(const char *url) {
    char s[256];
    strcpy(s, url);
    char **tokens = malloc(sizeof(char*) * 5); 
    char* token = strtok(s, "/");
    tokens[0] = malloc(sizeof(char) * 32);
    strcpy(tokens[0], token);
    int i=1;
    while (token) {
        token = strtok(NULL, "/");
        if(token) {
            tokens[i] = malloc(sizeof(char) * 32);
            strcpy(tokens[i], token);
            i++;
        }
    }
    
    url_t *result = (url_t *) malloc(sizeof(url_t));
    result->size = i;
    result->tokens = tokens;

    return result;
}

void print_from(const trie_node_t *root) {
    printf("%s -> %s -> %s -> %d\n", root->word, root->children[0]->word, root->children[0]->children[0]->word, root->children[0]->children[0]->children_size);
}

int add_endpoint(trie_node_t *root, const url_t *url) {
    trie_node_t *current_node = root;
    int current_token_i = 0;
    while(current_token_i < url->size) {
        int found = 0;

        //printf("current node (%s) has %d\n", current_node->word, current_node->children_size);
        for(int i=0; i<current_node->children_size; i++) {
            if(strcmp(current_node->children[i]->word, url->tokens[current_token_i]) == 0) {
                //printf("found\n");
                found = 1;
                trie_node_t *node_found = current_node->children[i];
                current_node = node_found;
                break;
            }
        }
        if (!found) {
                trie_node_t **children_list = (trie_node_t **) calloc(10, sizeof(trie_node_t*));
                trie_node_t *node_to_add = (trie_node_t *) malloc(sizeof(trie_node_t));
                char *word = (char *) malloc(64);
                strcpy(word, url->tokens[current_token_i]);
                node_to_add->word=word;
                if(current_token_i == url->size-1) {
                    node_to_add->is_leaf=true;
                } else {
                    node_to_add->is_leaf=false;
                }
                node_to_add->children_size=0;
                node_to_add->children=children_list;

                current_node->children[current_node->children_size] = node_to_add;
                current_node->children_size++;

                current_node = node_to_add;
        }
        current_token_i++;
    }
    print_from(root);
}


trie_node_t *prepare_trie() {
    trie_node_t *root = (trie_node_t *) malloc(sizeof(trie_node_t));
    trie_node_t **children = (trie_node_t **) malloc(sizeof(trie_node_t*) * 10);
    root->word = "/";
    root->children = children;
    root->is_leaf = false;
    root->children_size = 0;
    return root;
}

// int main() {
//     trie_node_t *root = prepare_trie();

//     const url_t *tokens = tokenize_url("/v1/domain/subdomain");
//     const url_t *tokens2 = tokenize_url("/v1/domain2/*");
//     const url_t *tokens3 = tokenize_url("/v1/domain2/toto");
//     const url_t *tokens4 = tokenize_url("/v1/domain/subdomain3");
//     const url_t *tokens5 = tokenize_url("/v1/domain");
//     const url_t *tokens6 = tokenize_url("/v1/domain/toto");
//     const url_t *tokens7 = tokenize_url("/v2/**");
//     const url_t *tokens8 = tokenize_url("/v2/domain/subdomain");
//     const url_t *tokens9 = tokenize_url("/v3/domain/subdomain");

//     add_endpoint(root, tokens);
//     add_endpoint(root, tokens2);
//     add_endpoint(root, tokens5);
//     add_endpoint(root, tokens7);
//     printf("---------\n");
//     if(find(root, tokens)) {
//         printf("TRUE\n");
//     } else {
//         printf("FALSE\n");
//     }
//     if(find(root, tokens5)) {
//         printf("FALSE\n");
//     } else {
//         printf("TRUE\n");
//     }
//     if(find(root, tokens3)) {
//         printf("TRUE\n");
//     } else {
//         printf("FALSE\n");
//     }
//     if(find(root, tokens4)) {
//         printf("FALSE\n");
//     } else {
//         printf("TRUE\n");
//     }
//     if(find(root, tokens6)) {
//         printf("FALSE\n");
//     } else {
//         printf("TRUE\n");
//     }
//     if(find(root, tokens8)) {
//         printf("TRUE\n");
//     } else {
//         printf("FALSE\n");
//     }
//     if (!find(root, tokens9)) {
//         printf("TRUE\n");
//     } else {
//         printf("FALSE\n");
//     }
// }