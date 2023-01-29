#include <string>
#include <vector>
#ifndef WHITELIST_HPP
#define WHITELIST_HPP

#ifdef __cplusplus 
extern "C" {
#endif

#include "trie_t.h"

#ifdef __cplusplus 
}
#endif

class Whitelist {
    public:
    static bool isUrlWhitelisted(trie_node_t *whitelist, std::string url);
    static trie_node_t *setWhitelist(std::string whitelist_filename);

    static bool isUrlWhitelistedList(std::vector<std::string> whitelist, std::string url);
    static std::vector<std::string> setWhitelistList(std::string whitelist_filename);
};

#endif