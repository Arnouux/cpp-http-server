#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "trie_t.h"


int find(trie_node_t root, const char **target) {
    if(strcmp(target[0], root.word)) {
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

const char **tokenize_url(const char *url) {
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
    const char **ctokens;
    ctokens = tokens;
    return ctokens;
}