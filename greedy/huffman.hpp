#pragma once
#include <map>
#include <memory>
#include <string>

// One node of the Huffman tree. Leaves hold a character; internal nodes do not.
struct HuffmanNode {
    int freq;
    char ch;                                   // only meaningful for leaves
    int order;                                 // creation number, used to break ties
    std::unique_ptr<HuffmanNode> left;         // bit '0'
    std::unique_ptr<HuffmanNode> right;        // bit '1'
    bool is_leaf() const { return !left && !right; }
};

// Count how often each character appears. O(n)
std::map<char, int> count_frequencies(const std::string& text);

// Greedy: repeatedly merge the two least frequent trees. O(k log k), k = distinct chars.
// Returns nullptr for an empty frequency table.
std::unique_ptr<HuffmanNode> build_huffman_tree(const std::map<char, int>& freq);

// Walk the tree: left = '0', right = '1'. A single-character tree gets code "0".
std::map<char, std::string> build_codes(const HuffmanNode* root);

// Text -> bit string ("0101..."). O(total bits)
std::string huffman_encode(const std::string& text, const std::map<char, std::string>& codes);

// Bit string -> text by walking the tree from the root. O(total bits)
std::string huffman_decode(const std::string& bits, const HuffmanNode* root);