#include "whitelist.hpp"
#include <fstream>
#include <vector>
#include <regex>

bool Whitelist::isUrlWhitelistedList(std::vector<std::string> whitelist, std::string url) {
    for(std::string u : whitelist) {
    const std::regex regex = std::regex(u);
        if(std::regex_search(url, regex)) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> Whitelist::setWhitelistList(std::string whitelist_filename) {
    std::vector<std::string> vec;
    std::string line;
    std::ifstream whitelist_file (whitelist_filename);
    if (whitelist_file.is_open()) {
        while ( getline (whitelist_file,line) ) {
            vec.push_back(line);
        }
        whitelist_file.close();
    }
    return vec;
}


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