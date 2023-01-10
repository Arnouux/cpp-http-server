#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"

typedef struct trie_node_t {
    char *word;
    bool is_leaf;
    struct trie_node_t *children;
    int children_size;
} trie_node_t;

trie_node_t root;

int find(char **target) {
    if(target[0] != root.word) {
        return 0;
    }

    trie_node_t current_node = root;
    int current_token_i = 1;

    int found = 1;
    while(found) {
        found = 0;
        for(int i=0; i<current_node.children_size; ++i) {
            printf("val: %s\n", current_node.children[i].word);
            printf("target: %s\n", target[current_token_i]);
            if(current_node.children[i].word == target[current_token_i]) {
                printf("found\n");
                current_token_i++;
                current_node = current_node.children[i];
                found = 1;
            }
        }
    }
    printf("end\n");
    return 0;
}

int main() {

    char *url1 = "/v1/discovery/services";
    char *url2 = "/v1/discovery/services/1";

    trie_node_t *children = (trie_node_t*) malloc(sizeof(trie_node_t) * 10);
    trie_node_t *children_discovery = (trie_node_t*) malloc(sizeof(trie_node_t) * 10);

    root.word = "/v1";
    root.is_leaf = false;
    root.children = children;

    trie_node_t discovery = { .word = "/discovery", .children = children_discovery, .is_leaf = false, .children_size = 1};
    children[0] = discovery;
    root.children_size++;

    trie_node_t marketrisk = { .word = "/marketrisk"};
    children[1] = marketrisk;
    root.children_size++;


    // discovery
    trie_node_t services = { .word = "/services"};
    children_discovery[0] = services;
    discovery.children_size++;


    char **tokens = (char **) malloc(sizeof(char*) * 3);
    tokens[0] = "/v1";
    tokens[1] = "/discovery";
    tokens[2] = "/services";
    find(tokens);

    return 0;
}