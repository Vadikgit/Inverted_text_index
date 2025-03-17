#ifndef HUFFMAN_CODE_PROCESSOR
#define HUFFMAN_CODE_PROCESSOR

#include <unordered_map>
#include <vector>
#include <string>

// TO DO:
template <class T>
struct CodeTreeNode
{
	CodeTreeNode *left0;
	CodeTreeNode *right1;

	T val;
	int64_t freq;
};

template <class T>
class HuffmanCodeProcessor
{
public:
	std::vector<CodeTreeNode<T> *> CodeTreeNodesAllocator;
	std::unordered_map<T, std::string> Codes;

	void HuffmanPrefixTreeTraversal(CodeTreeNode<T> *root, std::string &currentString);
	void AddStringToHuffmanPrefixTree(CodeTreeNode<T> *root, std::string &addingCode, T val);
	T getValueFromSequence(CodeTreeNode<T> *root, const std::vector<uint8_t> &data, int64_t &passedBitsForData, size_t currentPosInSrc);
	void prepareCodeTreeAndTable(const std::vector<T> &src);
	void encodeHuffman(const std::vector<T> &src, std::vector<uint8_t> &dst);
	void decodeHuffman(const std::vector<uint8_t> &src, std::vector<T> &dst, size_t numOfSymbolsToDecode);
	HuffmanCodeProcessor();
	~HuffmanCodeProcessor();
};

#endif