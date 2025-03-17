#include <iostream>
#include <algorithm>
#include <queue>
#include <memory>
#include "huffman_code_processor.h"

// #define SHOW_HUFFMAN_PROCESS

// TO DO:

template class HuffmanCodeProcessor<size_t>;

template <class T>
void HuffmanCodeProcessor<T>::HuffmanPrefixTreeTraversal(CodeTreeNode<T> *root, std::string &currentString)
{
	if (root->left0 == nullptr && root->right1 == nullptr)
	{
		Codes[root->val] = currentString;
	}

	if (root->left0 != nullptr)
	{
		currentString.push_back('0');
		HuffmanPrefixTreeTraversal(root->left0, currentString);
		currentString.pop_back();
	}

	if (root->right1 != nullptr)
	{
		currentString.push_back('1');
		HuffmanPrefixTreeTraversal(root->right1, currentString);
		currentString.pop_back();
	}
}

template <class T>
void HuffmanCodeProcessor<T>::AddStringToHuffmanPrefixTree(CodeTreeNode<T> *root, std::string &addingCode, T val)
{

	for (size_t i = 0; i < addingCode.length(); i++)
	{
		if (addingCode[i] == '0')
		{
			if (root->left0 == nullptr)
			{
				CodeTreeNodesAllocator.push_back(new CodeTreeNode<T>({nullptr, nullptr, T(), 0}));
				root->left0 = CodeTreeNodesAllocator.back();
			}

			root = root->left0;
		}
		else
		{
			if (root->right1 == nullptr)
			{
				CodeTreeNodesAllocator.push_back(new CodeTreeNode<T>({nullptr, nullptr, T(), 0}));
				root->right1 = CodeTreeNodesAllocator.back();
			}

			root = root->right1;
		}

		if (i == addingCode.length() - 1)
			root->val = val;
	}
}

template <class T>
T HuffmanCodeProcessor<T>::getValueFromSequence(CodeTreeNode<T> *root, const std::vector<uint8_t> &data, int64_t &passedBitsForData, size_t currentPosInSrc)
{
	while (!(root->left0 == nullptr && root->right1 == nullptr))
	{
		bool bit = (data[currentPosInSrc + passedBitsForData / 8] >> (passedBitsForData % 8)) % 2;

		if (bit == 0)
			root = root->left0;
		else
			root = root->right1;

		passedBitsForData++;
	}

	return root->val;
}

template <class T>
void HuffmanCodeProcessor<T>::prepareCodeTreeAndTable(const std::vector<T> &src)
{
	if (src.empty())
		return;

	std::unordered_map<T, uint32_t> numberOfSymbols;

	for (auto i : src)
	{
		if (numberOfSymbols.find(i) != numberOfSymbols.end())
			numberOfSymbols[i]++;
		else
			numberOfSymbols[i] = 1;
	}

#ifdef SHOW_HUFFMAN_PROCESS
	std::cout << "\n\nNumber of each val:\n";
	for (auto i : numberOfSymbols)
		std::cout << i.first << '\t' << i.second << '\n';
#endif // SHOW_HUFFMAN_PROCESS

	// create and fill priority queue
	std::priority_queue<CodeTreeNode<T>, std::vector<CodeTreeNode<T>>, std::greater<CodeTreeNode<T>>> priorityQueue;

	for (auto i : numberOfSymbols)
		priorityQueue.push({nullptr, nullptr, i.first, i.second});

	CodeTreeNodesAllocator.clear();

	if (numberOfSymbols.size() == 1)
	{
		CodeTreeNodesAllocator.push_back(new CodeTreeNode<T>({nullptr, nullptr, numberOfSymbols.begin()->first, numberOfSymbols.begin()->second}));
	}

	for (size_t i = 0; i < numberOfSymbols.size() - 1; i++)
	{
		CodeTreeNodesAllocator.push_back(new CodeTreeNode<T>({nullptr, nullptr, T(), 0}));

		auto x = priorityQueue.top();
		priorityQueue.pop();
		CodeTreeNodesAllocator.push_back(new CodeTreeNode<T>(x));

		x = priorityQueue.top();
		priorityQueue.pop();
		CodeTreeNodesAllocator.push_back(new CodeTreeNode<T>(x));

		(**(CodeTreeNodesAllocator.rbegin() + 2)).freq = (**(CodeTreeNodesAllocator.rbegin() + 1)).freq + (**(CodeTreeNodesAllocator.rbegin())).freq;

		(**(CodeTreeNodesAllocator.rbegin() + 2)).left0 = *(CodeTreeNodesAllocator.rbegin() + 1);
		(**(CodeTreeNodesAllocator.rbegin() + 2)).right1 = *(CodeTreeNodesAllocator.rbegin());

		priorityQueue.push(**(CodeTreeNodesAllocator.rbegin() + 2));
	}

	// priority queue now have only one element - root

#ifdef SHOW_HUFFMAN_PROCESS
	if (CodeTreeNodesAllocator.size() >= 3)
		std::cout << "\nSummary number of vals: " << (*(CodeTreeNodesAllocator.rbegin() + 2))->freq << '\n';
	else
		std::cout << "\nSummary number of vals: " << (*(CodeTreeNodesAllocator.rbegin()))->freq << '\n';

#endif // SHOW_HUFFMAN_PROCESS

	std::string codeBuffer;
	if (CodeTreeNodesAllocator.size() == 0)
		return;

	if (CodeTreeNodesAllocator.size() >= 3)
		HuffmanPrefixTreeTraversal(*(CodeTreeNodesAllocator.rbegin() + 2), codeBuffer);

	else
	{
		codeBuffer = "1";
		HuffmanPrefixTreeTraversal(*(CodeTreeNodesAllocator.rbegin()), codeBuffer);
	}

#ifdef SHOW_HUFFMAN_PROCESS
	std::cout << "\n\nHuffman codes:\n";
	for (auto i : Codes)
		std::cout << i.first << '\t' << i.second << '\n';
#endif // SHOW_HUFFMAN_PROCESS
}

template <class T>
void HuffmanCodeProcessor<T>::encodeHuffman(const std::vector<T> &src, std::vector<uint8_t> &dst)
{
	int64_t bitsNeedForEncodedData = 0;

	for (auto i : src)
		for (size_t j = 0; j < Codes[i].length(); j++)
			bitsNeedForEncodedData++;

	int64_t bytesNeedForData = ((bitsNeedForEncodedData + 7) / 8);

	dst.clear();
	dst.resize(bytesNeedForData);

	int passedBitsForData = 0;

	for (auto i : src)
	{
		for (size_t j = 0; j < Codes[i].length(); j++)
		{
			dst[passedBitsForData / 8] |= (uint8_t(Codes[i][j] - '0') << (passedBitsForData % 8));
			passedBitsForData++;
		}
	}
}

template <class T>
void HuffmanCodeProcessor<T>::decodeHuffman(const std::vector<uint8_t> &src, std::vector<T> &dst, size_t numOfSymbolsToDecode)
{

	if (src.empty())
		return;

	size_t currentPosInSrc = 0;

	dst.clear();
	dst.resize(numOfSymbolsToDecode);

	int64_t passedBitsForData = 0;

	for (size_t i = 0; i < numOfSymbolsToDecode; i++)
	{
		if (CodeTreeNodesAllocator.size() > 1)
			dst[i] = getValueFromSequence(*(CodeTreeNodesAllocator.rbegin() + 2), src, passedBitsForData, currentPosInSrc);
		else
			dst[i] = getValueFromSequence(*(CodeTreeNodesAllocator.rbegin()), src, passedBitsForData, currentPosInSrc);
	}
}

template <class T>
HuffmanCodeProcessor<T>::HuffmanCodeProcessor(){};

template <class T>
HuffmanCodeProcessor<T>::~HuffmanCodeProcessor()
{
	for (size_t i = 0; i < CodeTreeNodesAllocator.size(); i++)
	{
		// if (CodeTreeNodesAllocator[i] != nullptr);
		// delete CodeTreeNodesAllocator[i];
	}
}

template <class T>
bool operator<(const CodeTreeNode<T> &l, const CodeTreeNode<T> &r)
{
	return l.freq < r.freq;
}

template <class T>
bool operator==(const CodeTreeNode<T> &l, const CodeTreeNode<T> &r)
{
	return l.freq == r.freq;
}

template <class T>
bool operator>(const CodeTreeNode<T> &l, const CodeTreeNode<T> &r)
{
	return l.freq > r.freq;
}