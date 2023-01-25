#include "whitelist.hpp"
#include <fstream>

bool Whitelist::isUrlWhitelisted(trie_node_t *whitelist, std::string url) {
    const url_t *url_tokenized = tokenize_url(url.c_str());
    if(find(whitelist, url_tokenized)) {
        return true;
    }
    return false;
}

trie_node_t *Whitelist::setWhitelist(std::string whitelist_filename) {
    // root
    trie_node_t *whitelist = prepare_trie();

    // endpoints
    std::string line;
    std::ifstream whitelist_file (whitelist_filename);
    if (whitelist_file.is_open()) {
        while ( getline (whitelist_file,line) ) {
            const url_t *url = tokenize_url(line.c_str());
            add_endpoint(whitelist, url);
        }
        whitelist_file.close();
    }
    return whitelist;
}