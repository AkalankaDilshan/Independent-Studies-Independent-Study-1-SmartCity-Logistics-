#include "huffman.hpp"
#include <algorithm>
#include <vector>

std::map<char, int> count_frequencies(const std::string& text) {
    std::map<char, int> freq;
    for (char c : text) freq[c]++;
    return freq;
}

// Min-heap order: lower frequency first; equal frequency -> older node first.
// (std heap functions build a MAX-heap, so "greater" puts the smallest on top.)
static bool heap_greater(const std::unique_ptr<HuffmanNode>& a,
                         const std::unique_ptr<HuffmanNode>& b) {
    if (a->freq != b->freq) return a->freq > b->freq;
    return a->order > b->order;
}

std::unique_ptr<HuffmanNode> build_huffman_tree(const std::map<char, int>& freq) {
    std::vector<std::unique_ptr<HuffmanNode>> heap;
    int order = 0;

    // Step 1: one leaf per character
    for (const auto& [c, f] : freq) {
        auto leaf = std::make_unique<HuffmanNode>();
        leaf->freq = f;
        leaf->ch = c;
        leaf->order = order++;
        heap.push_back(std::move(leaf));
    }
    if (heap.empty()) return nullptr;                       // empty text
    std::make_heap(heap.begin(), heap.end(), heap_greater);

    // Step 2: greedy choice - merge the two smallest until one tree is left
    while (heap.size() > 1) {
        std::pop_heap(heap.begin(), heap.end(), heap_greater);
        std::unique_ptr<HuffmanNode> a = std::move(heap.back());   // smallest
        heap.pop_back();
        std::pop_heap(heap.begin(), heap.end(), heap_greater);
        std::unique_ptr<HuffmanNode> b = std::move(heap.back());   // second smallest
        heap.pop_back();

        auto parent = std::make_unique<HuffmanNode>();
        parent->freq = a->freq + b->freq;
        parent->ch = '\0';
        parent->order = order++;
        parent->left = std::move(a);
        parent->right = std::move(b);

        heap.push_back(std::move(parent));
        std::push_heap(heap.begin(), heap.end(), heap_greater);
    }
    return std::move(heap.front());                         // the root
}

static void collect_codes(const HuffmanNode* node, const std::string& prefix,
                          std::map<char, std::string>& codes) {
    if (node->is_leaf()) {
        codes[node->ch] = prefix.empty() ? "0" : prefix;    // single-char text
        return;
    }
    collect_codes(node->left.get(), prefix + "0", codes);
    collect_codes(node->right.get(), prefix + "1", codes);
}

std::map<char, std::string> build_codes(const HuffmanNode* root) {
    std::map<char, std::string> codes;
    if (root) collect_codes(root, "", codes);
    return codes;
}

std::string huffman_encode(const std::string& text, const std::map<char, std::string>& codes) {
    std::string bits;
    for (char c : text) bits += codes.at(c);
    return bits;
}

std::string huffman_decode(const std::string& bits, const HuffmanNode* root) {
    std::string out;
    if (!root) return out;
    if (root->is_leaf()) {                                   // single-char text: "0" per char
        out.assign(bits.size(), root->ch);
        return out;
    }
    const HuffmanNode* node = root;
    for (char b : bits) {
        node = (b == '0') ? node->left.get() : node->right.get();
        if (node->is_leaf()) {                               // reached a character
            out += node->ch;
            node = root;                                     // start again at the top
        }
    }
    return out;
}