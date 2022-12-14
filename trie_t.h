typedef struct trie_node_t {
    const char *word;
    bool is_leaf;
    struct trie_node_t *children;
    int children_size;
} trie_node_t;

int find(trie_node_t root, const char **target);
const char **tokenize_url(const char *url);