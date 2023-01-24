typedef struct trie_node_t {
    const char *word;
    bool is_leaf;
    struct trie_node_t **children;
    int children_size;
} trie_node_t;

typedef struct url_t {
    const char **tokens;
    int size;
} url_t;

int find(const trie_node_t *root, const url_t *url);
const url_t *tokenize_url(const char *url);
int add_endpoint(trie_node_t *root, const url_t *url);
trie_node_t *prepare_trie();