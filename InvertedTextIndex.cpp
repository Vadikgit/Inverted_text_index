// InvertedTextIndex.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <chrono>
#include <set>
#include <queue>
#include <unordered_set>
#include <memory>

const size_t skipListLinksProbogateRadix = 2;

//#define SIMPLE_BOOLSEARCH_EXAMPLE
//#define SIMPLE_OPTIMISED_BOOLSEARCH_EXAMPLE
//#define SIMPLE_OPTIMISED_SKIP_BOOLSEARCH_EXAMPLE

//#define BIGGER_BOOLSEARCH_EXAMPLE
//#define BIGGER_OPTIMISED_BOOLSEARCH_EXAMPLE
//#define BIGGER_OPTIMISED_SKIP_BOOLSEARCH_EXAMPLE
//#define HUFFMAN_TEST
//#define SHOW_OPTIMISED_POSTING_ENCODING
//#define SHOW_OPTIMISED_SKIP_POSTING_ENCODING
//#define SHOW_SKIP_LIST_IN_DEBUGGER

#define CORRECTNESS_TEST
//#define PERFORMANCE_TEST

//#define SHOW_DECODED_BLOCKS_EVERY_TIME
//#define SHOW_HUFFMAN_PROCESS

// TO DO:
template <class T>
struct CodeTreeNode
{
	CodeTreeNode* left0;
	CodeTreeNode* right1;

	T val;
	int64_t freq;
};

template <class T>
class HuffmanCodeProcessor
{
public:

	std::vector<CodeTreeNode<T>* > CodeTreeNodesAllocator;
	std::unordered_map<T, std::string> Codes;

	void HuffmanPrefixTreeTraversal(CodeTreeNode<T>* root, std::string& currentString) {
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

	void AddStringToHuffmanPrefixTree(CodeTreeNode<T>* root, std::string& addingCode, T val) {

		for (size_t i = 0; i < addingCode.length(); i++)
		{
			if (addingCode[i] == '0')
			{
				if (root->left0 == nullptr)
				{
					CodeTreeNodesAllocator.push_back(new CodeTreeNode<T>({ nullptr, nullptr, T(), 0 }));
					root->left0 = CodeTreeNodesAllocator.back();
				}

				root = root->left0;
			}
			else
			{
				if (root->right1 == nullptr)
				{
					CodeTreeNodesAllocator.push_back(new CodeTreeNode<T>({ nullptr, nullptr, T(), 0 }));
					root->right1 = CodeTreeNodesAllocator.back();
				}

				root = root->right1;
			}

			if (i == addingCode.length() - 1)
				root->val = val;
		}

	}

	T getValueFromSequence(CodeTreeNode<T>* root, const std::vector<uint8_t>& data, int64_t& passedBitsForData, size_t currentPosInSrc) {
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

	void prepareCodeTreeAndTable(const std::vector<T>& src)
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
			priorityQueue.push({ nullptr, nullptr, i.first, i.second });

		CodeTreeNodesAllocator.clear();

		if (numberOfSymbols.size() == 1)
		{
			CodeTreeNodesAllocator.push_back(new CodeTreeNode<T>({ nullptr, nullptr, numberOfSymbols.begin()->first,  numberOfSymbols.begin()->second }));
		}

		for (size_t i = 0; i < numberOfSymbols.size() - 1; i++)
		{
			CodeTreeNodesAllocator.push_back(new CodeTreeNode<T>({ nullptr, nullptr, T(), 0 }));

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

	void encodeHuffman(const std::vector<T>& src, std::vector<uint8_t>& dst) 
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

	void decodeHuffman(const std::vector<uint8_t>& src, std::vector<T>& dst, size_t numOfSymbolsToDecode) {

		if (src.empty())
			return;

		size_t currentPosInSrc = 0;

		dst.clear();
		dst.resize(numOfSymbolsToDecode);

		int64_t passedBitsForData = 0;

		for (size_t i = 0; i < numOfSymbolsToDecode; i++) {
			if (CodeTreeNodesAllocator.size() > 1)
				dst[i] = getValueFromSequence(*(CodeTreeNodesAllocator.rbegin() + 2), src, passedBitsForData, currentPosInSrc);
			else
				dst[i] = getValueFromSequence(*(CodeTreeNodesAllocator.rbegin()), src, passedBitsForData, currentPosInSrc);
		}
	}

	HuffmanCodeProcessor() {};

	~HuffmanCodeProcessor()
	{
		for (size_t i = 0; i < CodeTreeNodesAllocator.size(); i++)
		{
			//if (CodeTreeNodesAllocator[i] != nullptr);
				//delete CodeTreeNodesAllocator[i];
		}
	};
};

template<class T>
bool operator< (const CodeTreeNode<T>& l, const CodeTreeNode<T>& r) {
	return l.freq < r.freq;
}

template<class T>
bool operator== (const CodeTreeNode<T>& l, const CodeTreeNode<T>& r) {
	return l.freq == r.freq;
}

template<class T>
bool operator> (const CodeTreeNode<T>& l, const CodeTreeNode<T>& r) {
	return l.freq > r.freq;
}

//////////////////////////////////////////////////////////

size_t left_bin_search(size_t* arr, size_t l, size_t r, size_t val) {
	size_t rCopy = r;

	while (l < r)
	{
		size_t m = (l + r) / 2;
		if (arr[m] >= val)
		{
			r = m;
		}
		else
		{
			l = m + 1;
		}
	}

	if (arr[l] != val)
		return (rCopy + 1);

	return l;
}


class PostingList
{
public:
	PostingList() : _currentPos{ 0 } {};
	~PostingList() {};

	std::vector<size_t> _docIds; // 1 .. smth    0 - endOfList, not stored, but can be returned

	size_t _currentPos;

	size_t next();
	size_t advance(size_t docId);
	size_t current();

	void reset();
	void generateFromDocidsVector(std::vector<size_t>& docIds);
	void print();
	size_t sizeInBytes();

	void saveDocIdsInVector(std::vector<size_t>& dst);
};

size_t PostingList::next()
{
	if (_docIds.size() == 0)
		return 0;

	if (_currentPos + 1 + 1 > _docIds.size())
	{
		_currentPos = _docIds.size();
		return 0;
	}

	_currentPos++;

	return _docIds[_currentPos];
}

size_t PostingList::current()
{
	if (_docIds.size() == 0)
		return 0;

	if (_currentPos + 1 > _docIds.size())
	{
		_currentPos = _docIds.size();
		return 0;
	}

	return _docIds[_currentPos];
}

size_t PostingList::advance(size_t docId)
{
	auto lb = std::lower_bound(_docIds.begin(), _docIds.end(), docId);

	if (lb == _docIds.end())
	{
		_currentPos = _docIds.size();
		return 0;
	}

	_currentPos = lb - _docIds.begin();
	return (*lb);
}

void PostingList::reset()
{
	_currentPos = 0;
}

void PostingList::generateFromDocidsVector(std::vector<size_t>& docIds)
{
	_docIds.clear();
	_docIds.resize(docIds.size());

	for (size_t i = 0; i < docIds.size(); i++)
		_docIds[i] = docIds[i];

	reset();
}

void PostingList::print()
{
	for (auto docId : _docIds)
		std::cout << docId << ' ';
}

size_t PostingList::sizeInBytes()
{
	return _docIds.size() * sizeof(size_t);
}

void PostingList::saveDocIdsInVector(std::vector<size_t>& dst)
{
	reset();
	dst.clear();

	while (current() != 0)
	{
		dst.push_back(current());
		next();
	}

	reset();
}

//////////////////////////////////////////////////////////

struct DocIdsBlock
{
	size_t firstDocIdInBlock;
	std::vector<uint8_t> encodedDocIds;
	std::vector<size_t> notEncodedDeltas;
};

class OptimisedPostingList
{
public:
	size_t _docIdsInBlock;
	size_t _docIdsInPosting;
	size_t encodingExceptionValue;

	int64_t _numberOfCurrentDecodedBlock;
	std::vector<size_t> _decodedCurrentBlockDocIds;

	std::vector<DocIdsBlock> _docIdsBlocks;

	HuffmanCodeProcessor<size_t> _huffmanCodeProcessor;

	OptimisedPostingList(size_t docIdsInBlock = 128) : _docIdsInBlock{ docIdsInBlock }, _currentPos{ 0 }, _numberOfCurrentDecodedBlock{ -1 } {_decodedCurrentBlockDocIds.assign(_docIdsInBlock, 0); };
	~OptimisedPostingList() {};

	size_t _currentPos;

	size_t next();
	size_t advance(size_t docId);
	size_t current();

	void reset();
	void generateFromDocidsVector(std::vector<size_t>& docIds);
	void print();
	size_t sizeInBytes();

	void decodeBlock(size_t blockNumber);

	void saveDocIdsInVector(std::vector<size_t>& dst);
};

size_t OptimisedPostingList::next()
{
	if (_docIdsBlocks.size() == 0)
		return 0;

	if (_currentPos + 1 + 1 > _docIdsInPosting)
	{
		_currentPos = _docIdsInPosting;
		return 0;
	}

	_currentPos++;

	return current();
}

size_t OptimisedPostingList::current()
{
	if (_docIdsBlocks.size() == 0)
		return 0;

	if (_currentPos + 1 > _docIdsInPosting)
	{
		_currentPos = _docIdsInPosting;
		return 0;
	}

	if (_currentPos / _docIdsInBlock != _numberOfCurrentDecodedBlock)
	{
		decodeBlock(_currentPos / _docIdsInBlock);
		//_numberOfCurrentDecodedBlock = _currentPos / _docIdsInBlock;
	}

	return _decodedCurrentBlockDocIds[_currentPos % _docIdsInBlock];
}

size_t OptimisedPostingList::advance(size_t docId)
{
	if (_docIdsInPosting == 0)
		return 0;

	size_t l = 0;
	size_t r = _docIdsBlocks.size() - 1;

	while (l < r)
	{
		size_t m = (l + r + 1) / 2;
		if (_docIdsBlocks[m].firstDocIdInBlock <= docId)
			l = m;
		else
			r = m - 1;
	}

	decodeBlock(l);

	int64_t findedPos = -1;

	for (size_t i = 0; i < std::min(_docIdsInBlock, _docIdsInPosting - _docIdsInBlock * (l)); i++)
	{
		if (_decodedCurrentBlockDocIds[i] >= docId)
		{
			findedPos = i;
			break;
		}
	}

	if ((findedPos == -1) && (l + 1 < _docIdsBlocks.size()))
	{
		l++;
		decodeBlock(l);
		findedPos = 0;
	}

	if (findedPos == -1)
	{
		_currentPos = _docIdsInPosting;
		return 0;
	}

	_currentPos = l * _docIdsInBlock + findedPos;
	return _decodedCurrentBlockDocIds[findedPos];
}

void OptimisedPostingList::reset()
{
	_currentPos = 0;
}

void OptimisedPostingList::decodeBlock(size_t blockNumber)
{
	if (_numberOfCurrentDecodedBlock == blockNumber)
		return;

	if (_docIdsInPosting == 0)
		return;
	
	_numberOfCurrentDecodedBlock = blockNumber;

	DocIdsBlock& block = _docIdsBlocks[blockNumber];

	size_t numberOfDecodedDocIds = std::min(_docIdsInBlock, _docIdsInPosting - _docIdsInBlock * (blockNumber));
	
	for (auto& i : _decodedCurrentBlockDocIds)
		i = 0;

	if (numberOfDecodedDocIds > 0)
	{
		_decodedCurrentBlockDocIds[0] = block.firstDocIdInBlock;
		
		std::vector<size_t> huffmanDeodedDeltas;
		
		_huffmanCodeProcessor.decodeHuffman(block.encodedDocIds, huffmanDeodedDeltas, numberOfDecodedDocIds - 1);
		
		size_t nextExceptionDeltaPos = 0;
		size_t currentDelta;
		for (size_t i = 0; i < huffmanDeodedDeltas.size(); i++)
		{
			currentDelta = huffmanDeodedDeltas[i];
			if (huffmanDeodedDeltas[i] == encodingExceptionValue)
			{
				currentDelta = block.notEncodedDeltas[nextExceptionDeltaPos];
				nextExceptionDeltaPos++;
			}

			_decodedCurrentBlockDocIds[1 + i] = _decodedCurrentBlockDocIds[i] + currentDelta;
		}
	}
	
#ifdef SHOW_OPTIMISED_POSTING_ENCODING
#ifdef SHOW_DECODED_BLOCKS_EVERY_TIME
	std::cout << "\ndecoded " << numberOfDecodedDocIds << " docIds from block " << blockNumber << ":\t";

	for (size_t i = 0; i < _decodedCurrentBlockDocIds.size(); i++)
		std::cout << _decodedCurrentBlockDocIds[i] << " ";
	std::cout << "\n";

#endif // SHOW_DECODED_BLOCKS_EVERY_TIME
#endif // SHOW_OPTIMISED_POSTING_ENCODING

}

void OptimisedPostingList::generateFromDocidsVector(std::vector<size_t>& docIds)
{
#ifdef SHOW_OPTIMISED_POSTING_ENCODING
	std::cout << "\n\nOptimised posting before encoding:\n";

	for (size_t i = 0; i < docIds.size(); i++)
		std::cout << docIds[i] << ' ';

#endif // SHOW_OPTIMISED_POSTING_ENCODING

	_docIdsInPosting = docIds.size();

	_docIdsBlocks.clear();
	_docIdsBlocks.resize((_docIdsInPosting + _docIdsInBlock - 1) / _docIdsInBlock); // allocate enough blocks

	size_t previosDocId = 0;

	for (size_t i = 0; i < docIds.size(); i++) // transform docIds to deltas (excluding block-first)
	{
		if (i % _docIdsInBlock == 0)
		{
			_docIdsBlocks[i / _docIdsInBlock].firstDocIdInBlock = docIds[i];
			previosDocId = docIds[i];
		}
		else
		{
			docIds[i] = docIds[i] - previosDocId;
			previosDocId = previosDocId + docIds[i];
		}
	}

#ifdef SHOW_OPTIMISED_POSTING_ENCODING
	std::cout << "\n\nOptimised posting after Delta encoding:\n";

	for (size_t i = 0; i < docIds.size(); i++)
	{
		if (i % _docIdsInBlock == 0)
		{
			std::cout << "| [" << docIds[i] << "] ";
		}
		else
			std::cout << docIds[i] << ' ';
	}

#endif // SHOW_OPTIMISED_POSTING_ENCODING

	std::unordered_map<size_t, uint32_t> numberOfEachDelta;

	for (size_t i = 0; i < docIds.size(); i++) // count frequency of each delta
	{
		if (i % _docIdsInBlock != 0)
		{
			if (numberOfEachDelta.find(docIds[i]) != numberOfEachDelta.end())
				numberOfEachDelta[docIds[i]]++;
			else
				numberOfEachDelta[docIds[i]] = 1;
		}
	}

#ifdef SHOW_OPTIMISED_POSTING_ENCODING

	std::cout << "\n\nNumber of each Delta:\n";
	for (auto i : numberOfEachDelta)
		std::cout << '\t' << i.first << '\t' << i.second << '\n';
	std::cout << "\n\n";

#endif // SHOW_OPTIMISED_POSTING_ENCODING

	std::vector<size_t> deltas;
	deltas.resize(numberOfEachDelta.size()); // understand which delta are in 20% of rarest

	size_t counter = 0;
	size_t maxDelta = 0;

	for (auto i : numberOfEachDelta)
	{
		deltas[counter] = i.first;
		counter++;

		if (i.first > maxDelta)
			maxDelta = i.first;
	}

	encodingExceptionValue = maxDelta + 1;

	std::sort(deltas.begin(), deltas.end(), [&](size_t d1, size_t d2) {	return numberOfEachDelta[d1] < numberOfEachDelta[d2]; });


#ifdef SHOW_OPTIMISED_POSTING_ENCODING

	size_t _totalNumberOfDeltas = docIds.size() - ((docIds.size() + _docIdsInBlock - 1) / _docIdsInBlock);
	size_t _numberOfPassedDeltas = 0;
	bool _alreadyDone = false;

	std::cout << "\n\nDeltas from rarest to most frequent:\n";
	for (size_t i = 0; i < deltas.size(); i++)
	{
		_numberOfPassedDeltas += numberOfEachDelta[deltas[i]];
		std::cout << ' ' << deltas[i] << "\t(passed " << _numberOfPassedDeltas << " deltas = " << static_cast<double>(_numberOfPassedDeltas) / _totalNumberOfDeltas << ")\n";

		//if ((_numberOfPassedDeltas >= _totalNumberOfDeltas / 5) && (_alreadyDone == false))
		//{
		//	std::cout << "-----------------------------------\n";
		//	_alreadyDone = true;
		//}

		if ((i + 1 >= deltas.size() / 5) && (_alreadyDone == false))
		{
			std::cout << "-----------------------------------\n";
			_alreadyDone = true;
		}
	}

#endif // SHOW_OPTIMISED_POSTING_ENCODING

	size_t totalNumberOfDeltas = docIds.size() - ((docIds.size() + _docIdsInBlock - 1) / _docIdsInBlock);
	size_t numberOfPassedDeltas = 0;

	std::unordered_set<size_t> exceptionDeltas;

	for (size_t i = 0; i < deltas.size(); i++)
	{
		exceptionDeltas.insert(deltas[i]);
		numberOfPassedDeltas += numberOfEachDelta[deltas[i]];

//		if (numberOfPassedDeltas >= totalNumberOfDeltas / 5)
//			break;

		if ((i + 1 ) >= deltas.size() / 5)
			break;
	}

	for (size_t i = 0; i < docIds.size(); i++) // change excepted deltas for exception marker, insert original deltas to array
	{
		if (i % _docIdsInBlock != 0)
		{
			if (exceptionDeltas.find(docIds[i]) != exceptionDeltas.end())
			{
				_docIdsBlocks[i / _docIdsInBlock].notEncodedDeltas.push_back(docIds[i]);
				docIds[i] = encodingExceptionValue;
			}
		}
	}

#ifdef SHOW_OPTIMISED_POSTING_ENCODING
	std::cout << "\n\nExcepted deltas changed for exception marker: " << encodingExceptionValue << "\n";

	for (size_t i = 0; i < docIds.size(); i++)
	{
		if (i % _docIdsInBlock == 0)
		{
			std::cout << "| [" << docIds[i] << "] ";
		}
		else
			std::cout << docIds[i] << ' ';
	}

#endif // SHOW_OPTIMISED_POSTING_ENCODING

	std::vector<size_t> deltasWithoutBlockFirstDocIds;
	size_t numberOfDeltasWithoutBlockFirstDocIds = _docIdsInPosting - ((_docIdsInPosting + _docIdsInBlock - 1) / _docIdsInBlock);
	deltasWithoutBlockFirstDocIds.resize(numberOfDeltasWithoutBlockFirstDocIds);

	for (size_t i = 0; i < deltasWithoutBlockFirstDocIds.size(); i++)
		deltasWithoutBlockFirstDocIds[i] = docIds[i + ((i + 1 + _docIdsInBlock - 1 - 1) / (_docIdsInBlock - 1))];


#ifdef SHOW_OPTIMISED_POSTING_ENCODING
	std::cout << "\n\nDeltas without block-first doIds:\n";

	for (size_t i = 0; i < deltasWithoutBlockFirstDocIds.size(); i++)
	{
		if (i % (_docIdsInBlock - 1) == 0)
			std::cout << "| ";

		std::cout << deltasWithoutBlockFirstDocIds[i] << ' ';
	}

#endif // SHOW_OPTIMISED_POSTING_ENCODING

	_huffmanCodeProcessor.prepareCodeTreeAndTable(deltasWithoutBlockFirstDocIds); // construct data structures for Huffman encoding/decoding

	for (size_t i = 0; i < _docIdsBlocks.size(); i++) // Huffman encoding of deltas
	{
		std::vector<size_t> deltasForCurrentBlock(deltasWithoutBlockFirstDocIds.begin() + i * (_docIdsInBlock - 1),
			deltasWithoutBlockFirstDocIds.begin() + std::min(((i + 1) * (_docIdsInBlock - 1)), (deltasWithoutBlockFirstDocIds.size())));

		_huffmanCodeProcessor.encodeHuffman(deltasForCurrentBlock, _docIdsBlocks[i].encodedDocIds);
	}

#ifdef SHOW_OPTIMISED_POSTING_ENCODING
	std::cout << "\n\nHuffman encoded blocks:\n";

	for (size_t i = 0; i < _docIdsBlocks.size(); i++)
	{
		std::cout << "block " << i <<  " :\t";

		int64_t passedBitsForCodes = 0;
		for (size_t j = 0; j < std::min(_docIdsInBlock - 1, deltasWithoutBlockFirstDocIds.size() - (i * (_docIdsInBlock - 1))); j++)
		{
			size_t passedBitsForCodesCopy = passedBitsForCodes;

			size_t delta = _huffmanCodeProcessor.getValueFromSequence(*(_huffmanCodeProcessor.CodeTreeNodesAllocator.rbegin() + 2), _docIdsBlocks[i].encodedDocIds, passedBitsForCodes, 0);

			for (size_t k = passedBitsForCodesCopy; k < passedBitsForCodes; k++)
				std::cout << (_docIdsBlocks[i].encodedDocIds[k / 8] >> (k % 8)) % 2;
			
			std::cout << "(" << delta << ")-";
		}

		std::cout << ";\trequired " << _docIdsBlocks[i].encodedDocIds.size() << " bytes\n";
	}

#endif // SHOW_OPTIMISED_POSTING_ENCODING


	reset();
}

void OptimisedPostingList::print()
{
	reset();

	for (size_t i = 0; i < _docIdsInPosting; i++)
	{
		std::cout << current() << ' ';
		next();
	}

	reset();
}

size_t OptimisedPostingList::sizeInBytes()
{
	if (_docIdsBlocks.size() == 0)
		return 0;
	
	size_t res = 0;

	res +=  sizeof(_docIdsInBlock) + sizeof(_docIdsInPosting) + sizeof(encodingExceptionValue) + sizeof(_numberOfCurrentDecodedBlock) + _decodedCurrentBlockDocIds.size() * sizeof(size_t);

	for (size_t i = 0; i < _docIdsBlocks.size(); i++)
	{
		res += sizeof(_docIdsBlocks[i].firstDocIdInBlock) + _docIdsBlocks[i].encodedDocIds.size() + _docIdsBlocks[i].notEncodedDeltas.size() * sizeof(size_t);
	}

	for (auto & i : _huffmanCodeProcessor.Codes)
	{
		res += sizeof(i.first) + i.second.length();
	}

	for (size_t i = 0; i < _huffmanCodeProcessor.CodeTreeNodesAllocator.size(); i++)
	{
		res += sizeof(_huffmanCodeProcessor.CodeTreeNodesAllocator[i]);
	}

	res += sizeof(_currentPos);

	return res;
}

void OptimisedPostingList::saveDocIdsInVector(std::vector<size_t>& dst)
{
	reset();
	dst.clear();

	while (current() != 0)
	{
		dst.push_back(current());
		next();
	}

	reset();
}

//////////////////////////////////////////////////////////
template <class T>
class ScipListNode
{
public:
	T _value;
	
	std::vector<std::shared_ptr<ScipListNode>> _forwardLinks;
};

class OptimisedPostingListSkip
{
public:
	size_t _docIdsInBlock;
	size_t _docIdsInPosting;
	size_t encodingExceptionValue;

	std::shared_ptr<ScipListNode<DocIdsBlock>> _currentDecodedBlockPtr;
	std::vector<size_t> _decodedCurrentBlockDocIds;

	size_t _linksProbogateRadix;
	std::shared_ptr<ScipListNode<DocIdsBlock>> _firstDocIdBlockPtr;

	HuffmanCodeProcessor<size_t> _huffmanCodeProcessor;

	OptimisedPostingListSkip(size_t docIdsInBlock = 128) : _linksProbogateRadix { skipListLinksProbogateRadix }, _docIdsInBlock { docIdsInBlock }, _currentPos{ 0 } 
	{_decodedCurrentBlockDocIds.assign(_docIdsInBlock, 0); };
	~OptimisedPostingListSkip() {};

	size_t _currentPos;

	size_t next();
	size_t advance(size_t docId);
	size_t current();

	void reset();
	void generateFromDocidsVector(std::vector<size_t>& docIds);
	void print();
	size_t sizeInBytes();

	void decodeBlock(std::shared_ptr<ScipListNode<DocIdsBlock>> blockPtr);

	void saveDocIdsInVector(std::vector<size_t>& dst);
};

size_t OptimisedPostingListSkip::next()
{
	if (_docIdsInPosting == 0)
		return 0;

	if (_currentPos + 1 + 1 > _docIdsInPosting)
	{
		_currentPos = _docIdsInPosting;
		return 0;
	}

	_currentPos++;

	if (_currentPos % _docIdsInBlock == 0)
		decodeBlock(_currentDecodedBlockPtr.get()->_forwardLinks[0]);

	return current();
}

size_t OptimisedPostingListSkip::current()
{
	if (_docIdsInPosting == 0)
		return 0;

	if (_currentPos + 1 > _docIdsInPosting)
	{
		_currentPos = _docIdsInPosting;
		return 0;
	}

	if (bool(_currentDecodedBlockPtr) == false)
	{
		_currentDecodedBlockPtr = _firstDocIdBlockPtr;
		decodeBlock(_firstDocIdBlockPtr);
	}

	return _decodedCurrentBlockDocIds[_currentPos % _docIdsInBlock];
}

size_t OptimisedPostingListSkip::advance(size_t docId)
{
	if (_docIdsInPosting == 0)
		return 0;

	_currentPos = 0;
	size_t currentRadixPow = _currentDecodedBlockPtr.get()->_forwardLinks.size() - 1;
	size_t currentRadixPowValue = 1;
	
	for (size_t i = 1; i <= currentRadixPow; i++)
		currentRadixPowValue *= _linksProbogateRadix;
	
	bool possibleBlockFinded = false;

	std::shared_ptr<ScipListNode<DocIdsBlock>> ptr = _firstDocIdBlockPtr;

	while (true)
	{
		while ((bool(ptr.get()->_forwardLinks[currentRadixPow]) == true)
			&& (ptr.get()->_forwardLinks[currentRadixPow].get()->_value.firstDocIdInBlock <= docId))
		{
			ptr = ptr.get()->_forwardLinks[currentRadixPow];
			_currentPos += currentRadixPowValue * _docIdsInBlock;
		}

		if (bool(ptr.get()->_forwardLinks[0]) == false)
			break;
		
		if ((currentRadixPow == 0) && (ptr.get()->_forwardLinks[0].get()->_value.firstDocIdInBlock >= docId))
			break;

		currentRadixPow--;
		currentRadixPowValue /= _linksProbogateRadix;
	}

	decodeBlock(ptr);

	int64_t findedPos = -1;

	for (size_t i = 0; i < std::min(_docIdsInBlock, _docIdsInPosting - _currentPos); i++)
	{
		if (_decodedCurrentBlockDocIds[i] >= docId)
		{
			findedPos = i;
			break;
		}
	}

	if ((findedPos == -1) && (bool(ptr.get()->_forwardLinks[0]) == true))
	{
		ptr = ptr.get()->_forwardLinks[0];
		_currentPos += _docIdsInBlock;
		decodeBlock(ptr);
		findedPos = 0;
	}

	if (findedPos == -1)
	{
		_currentPos = _docIdsInPosting;
		return 0;
	}

	_currentPos = _currentPos + findedPos;
	return _decodedCurrentBlockDocIds[findedPos];
}

void OptimisedPostingListSkip::reset()
{
	decodeBlock(_firstDocIdBlockPtr);
	_currentDecodedBlockPtr = _firstDocIdBlockPtr;
	_currentPos = 0;
}

void OptimisedPostingListSkip::decodeBlock(std::shared_ptr<ScipListNode<DocIdsBlock>> blockPtr)
{
	if (_currentDecodedBlockPtr == blockPtr)//?????/////
		return;

	if (_docIdsInPosting == 0)
		return;

	_currentDecodedBlockPtr = blockPtr;

	size_t numberOfDecodedDocIds = _docIdsInBlock;
	
	if ((bool(blockPtr.get()->_forwardLinks[0]) == false) && (_docIdsInPosting % _docIdsInBlock != 0)) //last block and it is not full
	{
		numberOfDecodedDocIds = _docIdsInPosting % _docIdsInBlock;
	}

	for (auto& i : _decodedCurrentBlockDocIds)
		i = 0;

	if (numberOfDecodedDocIds > 0)
	{
		_decodedCurrentBlockDocIds[0] = blockPtr.get()->_value.firstDocIdInBlock;

		std::vector<size_t> huffmanDeodedDeltas;

		_huffmanCodeProcessor.decodeHuffman(blockPtr.get()->_value.encodedDocIds, huffmanDeodedDeltas, numberOfDecodedDocIds - 1);

		size_t nextExceptionDeltaPos = 0;
		size_t currentDelta;
		for (size_t i = 0; i < huffmanDeodedDeltas.size(); i++)
		{
			currentDelta = huffmanDeodedDeltas[i];
			if (huffmanDeodedDeltas[i] == encodingExceptionValue)
			{
				currentDelta = blockPtr.get()->_value.notEncodedDeltas[nextExceptionDeltaPos];
				nextExceptionDeltaPos++;
			}

			_decodedCurrentBlockDocIds[1 + i] = _decodedCurrentBlockDocIds[i] + currentDelta;
		}
	}

#ifdef SHOW_OPTIMISED_SKIP_POSTING_ENCODING
#ifdef SHOW_DECODED_BLOCKS_EVERY_TIME
	std::cout << "\ndecoded " << numberOfDecodedDocIds << " docIds from block " << blockPtr << ":\t";

	for (size_t i = 0; i < _decodedCurrentBlockDocIds.size(); i++)
		std::cout << _decodedCurrentBlockDocIds[i] << " ";
	std::cout << "\n";

#endif // SHOW_DECODED_BLOCKS_EVERY_TIME
#endif // SHOW_OPTIMISED_SKIP_POSTING_ENCODING

}

void OptimisedPostingListSkip::generateFromDocidsVector(std::vector<size_t>& docIds)
{
#ifdef SHOW_OPTIMISED_SKIP_POSTING_ENCODING
	std::cout << "\n\nOptimised posting before encoding:\n";

	for (size_t i = 0; i < docIds.size(); i++)
		std::cout << docIds[i] << ' ';

#endif // SHOW_OPTIMISED_SKIP_POSTING_ENCODING

	_docIdsInPosting = docIds.size();

	std::vector<std::shared_ptr<ScipListNode<DocIdsBlock>>> scipListNodesPointers;
	scipListNodesPointers.resize((_docIdsInPosting + _docIdsInBlock - 1) / _docIdsInBlock); // allocate enough blocks

	for (size_t i = 0; i < scipListNodesPointers.size(); i++)
		scipListNodesPointers[i] = std::make_shared<ScipListNode<DocIdsBlock>>();

	size_t logOfLenMinusOneFloor = 0;
	size_t blocksInPostingListLMinusOne = scipListNodesPointers.size() - 1;

	while (blocksInPostingListLMinusOne >= _linksProbogateRadix)
	{
		blocksInPostingListLMinusOne = blocksInPostingListLMinusOne / _linksProbogateRadix;
		logOfLenMinusOneFloor++;
	}


	for (size_t i = 0; i < scipListNodesPointers.size(); i++)
	{
		scipListNodesPointers[i].get()->_forwardLinks.assign(logOfLenMinusOneFloor + 1, std::shared_ptr<ScipListNode<DocIdsBlock>>());

		size_t radixPow = 1;
		for (size_t j = 0; j < logOfLenMinusOneFloor + 1; j++)
		{
			if (i + radixPow < scipListNodesPointers.size())
				scipListNodesPointers[i].get()->_forwardLinks[j] = scipListNodesPointers[i + radixPow];

			radixPow *= _linksProbogateRadix;
		}
	}


	size_t previosDocId = 0;

	for (size_t i = 0; i < docIds.size(); i++) // transform docIds to deltas (excluding block-first)
	{
		if (i % _docIdsInBlock == 0)
		{
			scipListNodesPointers[i / _docIdsInBlock].get()->_value.firstDocIdInBlock = docIds[i];
			previosDocId = docIds[i];
		}
		else
		{
			docIds[i] = docIds[i] - previosDocId;
			previosDocId = previosDocId + docIds[i];
		}
	}

#ifdef SHOW_OPTIMISED_SKIP_POSTING_ENCODING
	std::cout << "\n\nOptimised posting after Delta encoding:\n";

	for (size_t i = 0; i < docIds.size(); i++)
	{
		if (i % _docIdsInBlock == 0)
		{
			std::cout << "| [" << docIds[i] << "] ";
		}
		else
			std::cout << docIds[i] << ' ';
	}

#endif // SHOW_OPTIMISED_SKIP_POSTING_ENCODING

	std::unordered_map<size_t, uint32_t> numberOfEachDelta;

	for (size_t i = 0; i < docIds.size(); i++) // count frequency of each delta
	{
		if (i % _docIdsInBlock != 0)
		{
			if (numberOfEachDelta.find(docIds[i]) != numberOfEachDelta.end())
				numberOfEachDelta[docIds[i]]++;
			else
				numberOfEachDelta[docIds[i]] = 1;
		}
	}

#ifdef SHOW_OPTIMISED_SKIP_POSTING_ENCODING

	std::cout << "\n\nNumber of each Delta:\n";
	for (auto i : numberOfEachDelta)
		std::cout << '\t' << i.first << '\t' << i.second << '\n';
	std::cout << "\n\n";

#endif // SHOW_OPTIMISED_SKIP_POSTING_ENCODING

	std::vector<size_t> deltas;
	deltas.resize(numberOfEachDelta.size()); // understand which delta are in 20% of rarest

	size_t counter = 0;
	size_t maxDelta = 0;

	for (auto i : numberOfEachDelta)
	{
		deltas[counter] = i.first;
		counter++;

		if (i.first > maxDelta)
			maxDelta = i.first;
	}

	encodingExceptionValue = maxDelta + 1;

	std::sort(deltas.begin(), deltas.end(), [&](size_t d1, size_t d2) {	return numberOfEachDelta[d1] < numberOfEachDelta[d2]; });


#ifdef SHOW_OPTIMISED_SKIP_POSTING_ENCODING

	size_t _totalNumberOfDeltas = docIds.size() - ((docIds.size() + _docIdsInBlock - 1) / _docIdsInBlock);
	size_t _numberOfPassedDeltas = 0;
	bool _alreadyDone = false;

	std::cout << "\n\nDeltas from rarest to most frequent:\n";
	for (size_t i = 0; i < deltas.size(); i++)
	{
		_numberOfPassedDeltas += numberOfEachDelta[deltas[i]];
		std::cout << ' ' << deltas[i] << "\t(passed " << _numberOfPassedDeltas << " deltas = " << static_cast<double>(_numberOfPassedDeltas) / _totalNumberOfDeltas << ")\n";

		//if ((_numberOfPassedDeltas >= _totalNumberOfDeltas / 5) && (_alreadyDone == false))
		//{
		//	std::cout << "-----------------------------------\n";
		//	_alreadyDone = true;
		//}

		if ((i + 1 >= deltas.size() / 5) && (_alreadyDone == false))
		{
			std::cout << "-----------------------------------\n";
			_alreadyDone = true;
		}
	}

#endif // SHOW_OPTIMISED_SKIP_POSTING_ENCODING

	size_t totalNumberOfDeltas = docIds.size() - ((docIds.size() + _docIdsInBlock - 1) / _docIdsInBlock);
	size_t numberOfPassedDeltas = 0;

	std::unordered_set<size_t> exceptionDeltas;

	for (size_t i = 0; i < deltas.size(); i++)
	{
		exceptionDeltas.insert(deltas[i]);
		numberOfPassedDeltas += numberOfEachDelta[deltas[i]];

		//		if (numberOfPassedDeltas >= totalNumberOfDeltas / 5)
		//			break;

		if ((i + 1) >= deltas.size() / 5)
			break;
	}

	for (size_t i = 0; i < docIds.size(); i++) // change excepted deltas for exception marker, insert original deltas to array
	{
		if (i % _docIdsInBlock != 0)
		{
			if (exceptionDeltas.find(docIds[i]) != exceptionDeltas.end())
			{
				scipListNodesPointers[i / _docIdsInBlock].get()->_value.notEncodedDeltas.push_back(docIds[i]);
				docIds[i] = encodingExceptionValue;
			}
		}
	}

#ifdef SHOW_OPTIMISED_SKIP_POSTING_ENCODING
	std::cout << "\n\nExcepted deltas changed for exception marker: " << encodingExceptionValue << "\n";

	for (size_t i = 0; i < docIds.size(); i++)
	{
		if (i % _docIdsInBlock == 0)
		{
			std::cout << "| [" << docIds[i] << "] ";
		}
		else
			std::cout << docIds[i] << ' ';
	}

#endif // SHOW_OPTIMISED_SKIP_POSTING_ENCODING

	std::vector<size_t> deltasWithoutBlockFirstDocIds;
	size_t numberOfDeltasWithoutBlockFirstDocIds = _docIdsInPosting - ((_docIdsInPosting + _docIdsInBlock - 1) / _docIdsInBlock);
	deltasWithoutBlockFirstDocIds.resize(numberOfDeltasWithoutBlockFirstDocIds);

	for (size_t i = 0; i < deltasWithoutBlockFirstDocIds.size(); i++)
		deltasWithoutBlockFirstDocIds[i] = docIds[i + ((i + 1 + _docIdsInBlock - 1 - 1) / (_docIdsInBlock - 1))];


#ifdef SHOW_OPTIMISED_SKIP_POSTING_ENCODING
	std::cout << "\n\nDeltas without block-first doIds:\n";

	for (size_t i = 0; i < deltasWithoutBlockFirstDocIds.size(); i++)
	{
		if (i % (_docIdsInBlock - 1) == 0)
			std::cout << "| ";

		std::cout << deltasWithoutBlockFirstDocIds[i] << ' ';
	}

#endif // SHOW_OPTIMISED_SKIP_POSTING_ENCODING

	_huffmanCodeProcessor.prepareCodeTreeAndTable(deltasWithoutBlockFirstDocIds); // construct data structures for Huffman encoding/decoding

	for (size_t i = 0; i < scipListNodesPointers.size(); i++) // Huffman encoding of deltas
	{
		std::vector<size_t> deltasForCurrentBlock(deltasWithoutBlockFirstDocIds.begin() + i * (_docIdsInBlock - 1),
			deltasWithoutBlockFirstDocIds.begin() + std::min(((i + 1) * (_docIdsInBlock - 1)), (deltasWithoutBlockFirstDocIds.size())));

		_huffmanCodeProcessor.encodeHuffman(deltasForCurrentBlock, scipListNodesPointers[i].get()->_value.encodedDocIds);
	}

#ifdef SHOW_OPTIMISED_SKIP_POSTING_ENCODING
	std::cout << "\n\nHuffman encoded blocks:\n";

	for (size_t i = 0; i < scipListNodesPointers.size(); i++)
	{
		std::cout << "block " << i << " :\t";

		int64_t passedBitsForCodes = 0;
		for (size_t j = 0; j < std::min(_docIdsInBlock - 1, deltasWithoutBlockFirstDocIds.size() - (i * (_docIdsInBlock - 1))); j++)
		{
			size_t passedBitsForCodesCopy = passedBitsForCodes;

			size_t delta = _huffmanCodeProcessor.getValueFromSequence(*(_huffmanCodeProcessor.CodeTreeNodesAllocator.rbegin() + 2), scipListNodesPointers[i].get()->_value.encodedDocIds, passedBitsForCodes, 0);

			for (size_t k = passedBitsForCodesCopy; k < passedBitsForCodes; k++)
				std::cout << (scipListNodesPointers[i].get()->_value.encodedDocIds[k / 8] >> (k % 8)) % 2;

			std::cout << "(" << delta << ")-";
		}

		std::cout << ";\trequired " << scipListNodesPointers[i].get()->_value.encodedDocIds.size() << " bytes\n";
	}

#endif // SHOW_OPTIMISED_SKIP_POSTING_ENCODING
	if (scipListNodesPointers.size() > 0)
		_firstDocIdBlockPtr = scipListNodesPointers[0];

	reset();
}

void OptimisedPostingListSkip::print()
{
	reset();

	for (size_t i = 0; i < _docIdsInPosting; i++)
	{
		std::cout << current() << ' ';
		next();
	}
	
	reset();
}

size_t OptimisedPostingListSkip::sizeInBytes()
{
	if (_docIdsInPosting == 0)
		return 0;

	size_t res = 0;

	res += sizeof(_docIdsInBlock) + sizeof(_docIdsInPosting) + sizeof(encodingExceptionValue) + sizeof(_currentDecodedBlockPtr) + sizeof(_linksProbogateRadix) + sizeof(_firstDocIdBlockPtr) + _decodedCurrentBlockDocIds.size() * sizeof(size_t);

	auto ptr = _firstDocIdBlockPtr;

	while(bool(ptr) == true)
	{
		res += sizeof(ptr.get()->_value.firstDocIdInBlock) + ptr.get()->_value.encodedDocIds.size() + ptr.get()->_value.notEncodedDeltas.size() * sizeof(size_t);
		res += ptr.get()->_forwardLinks.size() * sizeof(ptr.get()->_forwardLinks[0]);

		ptr = ptr.get()->_forwardLinks[0];
	}

	for (auto& i : _huffmanCodeProcessor.Codes)
	{
		res += sizeof(i.first) + i.second.length();
	}

	for (size_t i = 0; i < _huffmanCodeProcessor.CodeTreeNodesAllocator.size(); i++)
	{
		res += sizeof(_huffmanCodeProcessor.CodeTreeNodesAllocator[i]);
	}

	res += sizeof(_currentPos);

	return res;
}

void OptimisedPostingListSkip::saveDocIdsInVector(std::vector<size_t>& dst)
{
	reset();
	dst.clear();

	while (current() != 0)
	{
		dst.push_back(current());
		next();
	}

	reset();
}

//////////////////////////////////////////////////////////
template <class PostingListType>
class InvertedTextIndex
{
public:
	std::unordered_map<std::string, PostingListType> _termsPostingLists;

	PostingListType queryOR(std::vector<std::string> terms);
	PostingListType queryOR(std::vector<PostingListType> postingLists);

	PostingListType queryAND(std::vector<std::string> terms);
	PostingListType queryAND(std::vector<PostingListType> postingLists);

	PostingListType filterNO(std::vector<std::string> terms, std::vector<std::string> termsToDiscard);
	PostingListType filterNO(std::vector<std::string> terms, std::vector<PostingListType> postingListsToDiscard);
	PostingListType filterNO(std::vector<PostingListType> postingLists, std::vector<std::string> termsToDiscard);
	PostingListType filterNO(std::vector<PostingListType> postingLists, std::vector<PostingListType> postingListsToDiscard);

	InvertedTextIndex();
	~InvertedTextIndex();

	void print();

	size_t sizeOfIndexInBytes();
};

template <class PostingListType>
InvertedTextIndex<PostingListType>::InvertedTextIndex()
{
}
template <class PostingListType>
InvertedTextIndex<PostingListType>::~InvertedTextIndex()
{
}

template <class PostingListType>
PostingListType InvertedTextIndex<PostingListType>::queryOR(std::vector<std::string> terms)
{
	std::vector<PostingListType> postingLists;

	for (auto& term : terms)
	{
		auto iter = _termsPostingLists.find(term);

		if (iter != _termsPostingLists.end())
		{
			postingLists.push_back((*iter).second);
		}
	}

	return queryOR(postingLists);
}

template <class PostingListType>
PostingListType InvertedTextIndex<PostingListType>::queryOR(std::vector<PostingListType> postingLists)
{
	std::set<size_t> relevantDocIds = {};

	for (auto & posting : postingLists)
	{
		posting.reset();

		while (posting.current() != 0)
		{
			relevantDocIds.insert(posting.current());
			posting.next();
		}
	}

	std::vector<size_t> resDocIds;

	for (auto i : relevantDocIds)
		resDocIds.push_back(i);

	PostingListType res;
	res.generateFromDocidsVector(resDocIds);

	return res;
}

template <class PostingListType>
PostingListType InvertedTextIndex<PostingListType>::queryAND(std::vector<std::string> terms)
{
	std::vector<PostingListType> postingLists;

	for (auto& term : terms)
	{
		auto iter = _termsPostingLists.find(term);

		if (iter != _termsPostingLists.end())
		{
			postingLists.push_back((*iter).second);
		}
	}

	if (postingLists.size() != terms.size())
		return PostingListType();

	return queryAND(postingLists);
}

template <class PostingListType>
PostingListType InvertedTextIndex<PostingListType>::queryAND(std::vector<PostingListType> postingLists)
{
	std::set<size_t> relevantDocIds = {};

	size_t currentMaxDocId = 0;
	
	for (auto& posting : postingLists)
	{
		posting.reset();

		if (posting.current() > currentMaxDocId)
			currentMaxDocId = posting.current();
	}

	if (currentMaxDocId == 0)
		return PostingListType();
	
	bool atLeastOneEndOfPostingListReached = false;

	while (atLeastOneEndOfPostingListReached == false)
	{
		for (auto& posting : postingLists)
		{
			if (posting.current() > currentMaxDocId)
				currentMaxDocId = posting.current();
		}

		for (auto& posting : postingLists)
			posting.advance(currentMaxDocId);

		bool allEqual = true;

		size_t valToCompare = postingLists[0].current();

		for (auto& posting : postingLists)
		{
			if (posting.current() == 0)
			{
				atLeastOneEndOfPostingListReached = true;
				break;
			}
		}

		for (auto& posting : postingLists)
		{
			if (posting.current() != valToCompare)
			{
				allEqual = false;
				break;
			}
		}

		if ((allEqual == true) && (atLeastOneEndOfPostingListReached == false))
		{
			relevantDocIds.insert(valToCompare);

			postingLists[0].next();

			if (atLeastOneEndOfPostingListReached == false)
				atLeastOneEndOfPostingListReached = (postingLists[0].current() == 0);
		}
	}

	std::vector<size_t> resDocIds;

	for (auto i : relevantDocIds)
		resDocIds.push_back(i);

	PostingListType res;
	res.generateFromDocidsVector(resDocIds);

	return res;
}

template <class PostingListType>
PostingListType InvertedTextIndex<PostingListType>::filterNO(std::vector<std::string> terms, std::vector<std::string> termsToDiscard)
{
	std::vector<PostingListType> postingLists;
	std::vector<PostingListType> postingListsToDiscard;

	for (auto& term : terms)
	{
		auto iter = _termsPostingLists.find(term);

		if (iter != _termsPostingLists.end())
		{
			postingLists.push_back((*iter).second);
		}
	}

	for (auto& term : termsToDiscard)
	{
		auto iter = _termsPostingLists.find(term);

		if (iter != _termsPostingLists.end())
		{
			postingListsToDiscard.push_back((*iter).second);
		}
	}

	return filterNO(postingLists, postingListsToDiscard);
}

template <class PostingListType>
PostingListType InvertedTextIndex<PostingListType>::filterNO(std::vector<std::string> terms, std::vector<PostingListType> postingListsToDiscard)
{
	std::vector<PostingListType> postingLists;

	for (auto& term : terms)
	{
		auto iter = _termsPostingLists.find(term);

		if (iter != _termsPostingLists.end())
		{
			postingLists.push_back((*iter).second);
		}
	}

	return filterNO(postingLists, postingListsToDiscard);
}

template <class PostingListType>
PostingListType InvertedTextIndex<PostingListType>::filterNO(std::vector<PostingListType> postingLists, std::vector<std::string> termsToDiscard)
{
	std::vector<PostingListType> postingListsToDiscard;

	for (auto& term : termsToDiscard)
	{
		auto iter = _termsPostingLists.find(term);

		if (iter != _termsPostingLists.end())
		{
			postingListsToDiscard.push_back((*iter).second);
		}
	}

	return filterNO(postingLists, postingListsToDiscard);
}

template <class PostingListType>
PostingListType InvertedTextIndex<PostingListType>::filterNO(std::vector<PostingListType> postingLists, std::vector<PostingListType> postingListsToDiscard)
{
	std::set<size_t> postingListsDocIds = {};

	for (auto& posting : postingLists)
	{
		posting.reset();

		while (posting.current() != 0)
		{
			postingListsDocIds.insert(posting.current());
			posting.next();
		}

	}

	std::set<size_t> postingListsToDiscardDocIds = {};

	for (auto& posting : postingListsToDiscard)
	{
		posting.reset();

		while (posting.current() != 0)
		{
			postingListsToDiscardDocIds.insert(posting.current());
			posting.next();
		}
	}


	for (auto discardDocId : postingListsToDiscardDocIds)
	{
		postingListsDocIds.erase(discardDocId);
	}

	std::vector<size_t> resDocIds;

	for (auto i : postingListsDocIds)
		resDocIds.push_back(i);

	PostingListType res;
	res.generateFromDocidsVector(resDocIds);

	return res;
}

template <class PostingListType>
void InvertedTextIndex<PostingListType>::print()
{
	for (auto & termPosting : _termsPostingLists)
	{
		std::cout << termPosting.first << " :\t";
		
		termPosting.second.print();
		std::cout << '\n';
	}
	std::cout << '\n';
}

template <class PostingListType>
size_t InvertedTextIndex<PostingListType>::sizeOfIndexInBytes()
{
	size_t res = 0;

	for (auto& termPosting : _termsPostingLists)
	{
		res += termPosting.first.length();
		res += termPosting.second.sizeInBytes();
	}

	return res;
}

template <class PostingListType>
void generateRandomPostings(InvertedTextIndex<PostingListType>& index, size_t numOfTerms, size_t minTermLen, size_t maxTermLen, size_t maxDocId, size_t maxPostingListLength)
{
	std::string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	index._termsPostingLists.clear();

	std::vector<size_t> ids;
	ids.resize(maxDocId);

	for (size_t i = 0; i < ids.size(); i++)
		ids[i] = i + 1;


	for (size_t i = 0; i < numOfTerms; i++)
	{
		size_t curTermLen = minTermLen + (rand() % (maxTermLen - minTermLen + 1));

		std::string curTerm = "";

		for (size_t j = 0; j < curTermLen; j++)
			curTerm = curTerm + letters[rand() % letters.length()];

		size_t curPostingLen = 1 + (rand() % maxPostingListLength);

		std::random_shuffle(ids.begin(), ids.end());

		std::vector<size_t> postingList(ids.begin(), ids.begin() + curPostingLen);
		std::sort(postingList.begin(), postingList.end());

		index._termsPostingLists[curTerm] = {};
		index._termsPostingLists[curTerm].generateFromDocidsVector(postingList);
	}
}


int main()
{
#ifdef SIMPLE_BOOLSEARCH_EXAMPLE

	InvertedTextIndex<PostingList> index;

	generateRandomPostings(index, 10, 3, 5, 50, 20);

	index.print();

	std::cout << "\n\nsize of index: " << index.sizeOfIndexInBytes() << " bytes\n\n";

	for (size_t i = 0; i < index._termsPostingLists["buSB"]._docIds.size() + 5; i++)
	{
		std::cout << "cur: " << index._termsPostingLists["buSB"].current();
		std::cout << ", next: " << index._termsPostingLists["buSB"].next() << ";\t";
	}

	index._termsPostingLists["buSB"].reset();

	std::cout << "\n\n";

	std::cout << "advance 14: " << index._termsPostingLists["buSB"].advance(14);
	std::cout << ", cur: " << index._termsPostingLists["buSB"].current() << ";\t";

	std::cout << "advance 22: " << index._termsPostingLists["buSB"].advance(22);
	std::cout << ", cur: " << index._termsPostingLists["buSB"].current() << ";\t";

	std::cout << "advance 30: " << index._termsPostingLists["buSB"].advance(30);
	std::cout << ", cur: " << index._termsPostingLists["buSB"].current() << ";\t";

	std::cout << "advance 44: " << index._termsPostingLists["buSB"].advance(44);
	std::cout << ", cur: " << index._termsPostingLists["buSB"].current() << ";\t";

	std::cout << "advance 54: " << index._termsPostingLists["buSB"].advance(54);
	std::cout << ", cur: " << index._termsPostingLists["buSB"].current() << ";\t";

	std::cout << "\n\n";

	auto orRes = index.queryOR({ "buSB", "ooeFx", "mnM" });

	std::cout << "\nOR (\"buSB\", \"ooeFx\", \"mnM\"):\t";

	orRes.print();
	std::cout << "\n";


	auto andRes = index.queryAND(std::vector<std::string>{ "buSB", "ooeFx", "uGgGe"});

	std::cout << "\nAND (\"buSB\", \"ooeFx\", \"uGgGe\"):\t";

	andRes.print();
	std::cout << "\n";

	auto notRes = index.filterNO(std::vector<PostingList>{ orRes}, std::vector<std::string>{ "buSB", "ooeFx"});

	std::cout << "\nNOT( OR (\"buSB\", \"ooeFx\", \"mnM\"), (\"buSB\", \"ooeFx\")):\t";

	notRes.print();
	std::cout << "\n";
#endif // SIMPLE_BOOLSEARCH_EXAMPLE

#ifdef SIMPLE_OPTIMISED_BOOLSEARCH_EXAMPLE

	InvertedTextIndex<OptimisedPostingList> index;

	generateRandomPostings(index, 10, 3, 5, 50, 20);

	index.print();

	std::cout << "\n\nsize of index: " << index.sizeOfIndexInBytes() << " bytes\n\n";

	for (size_t i = 0; i < index._termsPostingLists["buSB"]._docIdsInPosting + 5; i++)
	{
		std::cout << "cur: " << index._termsPostingLists["buSB"].current();
		std::cout << ", next: " << index._termsPostingLists["buSB"].next() << ";\t";
	}

	index._termsPostingLists["buSB"].reset();

	std::cout << "\n\n";

	std::cout << "advance 14: " << index._termsPostingLists["buSB"].advance(14);
	std::cout << ", cur: " << index._termsPostingLists["buSB"].current() << ";\t";

	std::cout << "advance 22: " << index._termsPostingLists["buSB"].advance(22);
	std::cout << ", cur: " << index._termsPostingLists["buSB"].current() << ";\t";

	std::cout << "advance 30: " << index._termsPostingLists["buSB"].advance(30);
	std::cout << ", cur: " << index._termsPostingLists["buSB"].current() << ";\t";

	std::cout << "advance 44: " << index._termsPostingLists["buSB"].advance(44);
	std::cout << ", cur: " << index._termsPostingLists["buSB"].current() << ";\t";

	std::cout << "advance 54: " << index._termsPostingLists["buSB"].advance(54);
	std::cout << ", cur: " << index._termsPostingLists["buSB"].current() << ";\t";

	std::cout << "\n";

	auto orRes = index.queryOR({ "buSB", "ooeFx", "mnM" });

	std::cout << "\nOR (\"buSB\", \"ooeFx\", \"mnM\"):\t";

	orRes.print();
	std::cout << "\n";


	auto andRes = index.queryAND(std::vector<std::string>{ "buSB", "ooeFx", "uGgGe"});

	std::cout << "\nAND (\"buSB\", \"ooeFx\", \"uGgGe\"):\t";

	andRes.print();
	std::cout << "\n";

	auto notRes = index.filterNO(std::vector<OptimisedPostingList>{ orRes}, std::vector<std::string>{ "buSB", "ooeFx"});

	std::cout << "\nNOT( OR (\"buSB\", \"ooeFx\", \"mnM\"), (\"buSB\", \"ooeFx\")):\t";

	notRes.print();
	std::cout << "\n";
#endif // SIMPLE_OPTIMISED_BOOLSEARCH_EXAMPLE

#ifdef SIMPLE_OPTIMISED_SKIP_BOOLSEARCH_EXAMPLE

	InvertedTextIndex<OptimisedPostingListSkip> index;

	generateRandomPostings(index, 10, 3, 5, 50, 20);

	index.print();

	std::cout << "\n\nsize of index: " << index.sizeOfIndexInBytes() << " bytes\n\n";

	for (size_t i = 0; i < index._termsPostingLists["buSB"]._docIdsInPosting + 5; i++)
	{
		std::cout << "cur: " << index._termsPostingLists["buSB"].current();
		std::cout << ", next: " << index._termsPostingLists["buSB"].next() << ";\t";
	}

	index._termsPostingLists["buSB"].reset();

	std::cout << "\n\n";

	std::cout << "advance 14: " << index._termsPostingLists["buSB"].advance(14);
	std::cout << ", cur: " << index._termsPostingLists["buSB"].current() << ";\t";

	std::cout << "advance 22: " << index._termsPostingLists["buSB"].advance(22);
	std::cout << ", cur: " << index._termsPostingLists["buSB"].current() << ";\t";

	std::cout << "advance 30: " << index._termsPostingLists["buSB"].advance(30);
	std::cout << ", cur: " << index._termsPostingLists["buSB"].current() << ";\t";

	std::cout << "advance 44: " << index._termsPostingLists["buSB"].advance(44);
	std::cout << ", cur: " << index._termsPostingLists["buSB"].current() << ";\t";

	std::cout << "advance 54: " << index._termsPostingLists["buSB"].advance(54);
	std::cout << ", cur: " << index._termsPostingLists["buSB"].current() << ";\t";

	std::cout << "\n";

	auto orRes = index.queryOR({ "buSB", "ooeFx", "mnM" });

	std::cout << "\nOR (\"buSB\", \"ooeFx\", \"mnM\"):\t";

	orRes.print();
	std::cout << "\n";


	auto andRes = index.queryAND(std::vector<std::string>{ "buSB", "ooeFx", "uGgGe"});

	std::cout << "\nAND (\"buSB\", \"ooeFx\", \"uGgGe\"):\t";

	andRes.print();
	std::cout << "\n";

	auto notRes = index.filterNO(std::vector<OptimisedPostingListSkip>{ orRes}, std::vector<std::string>{ "buSB", "ooeFx"});

	std::cout << "\nNOT( OR (\"buSB\", \"ooeFx\", \"mnM\"), (\"buSB\", \"ooeFx\")):\t";

	notRes.print();
	std::cout << "\n";
#endif // SIMPLE_OPTIMISED_SKIP_BOOLSEARCH_EXAMPLE

#ifdef BIGGER_BOOLSEARCH_EXAMPLE

	std::chrono::time_point<std::chrono::system_clock> t1, t2;

	InvertedTextIndex<PostingList> index;


	t1 = std::chrono::system_clock::now();

	generateRandomPostings(index, 3000, 3, 8, 10000, 3000);

	t2 = std::chrono::system_clock::now();

	std::cout << "\n" << " generating: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms;\nsize: " << index.sizeOfIndexInBytes() << " bytes";

	size_t numOfTerms = 0;
	size_t numOfDocids = 0;

	for (auto& term : index._termsPostingLists)
	{
		numOfTerms++;
		numOfDocids += term.second._docIds.size();
	}

	std::cout << "\nnumber of terms: " << numOfTerms << "\nnumber of docids: " << numOfDocids << " (" << static_cast<double>(index.sizeOfIndexInBytes()) / numOfDocids << " bytes/docId)\n\n";

	size_t counter = 3;

	std::vector<std::pair<std::string, PostingList>> somePostings;

	for (auto& term : index._termsPostingLists)
	{
		if (counter == 0)
			break;

		somePostings.push_back(term);

		std::cout << term.first << ": ";

		term.second.print();

		std::cout << "\n";

		counter--;
	}


	std::vector<std::string> termsForQuery;
	for (auto& term : somePostings)
	{
		termsForQuery.push_back(term.first);
	}

	auto andRes = index.queryAND(termsForQuery);

	std::cout << "\nAND (";

	for (auto& term : termsForQuery)
	{
		std::cout << "\"" << term << "\", ";
	}

	std::cout << "):\t";

	andRes.print();

#endif // BIGGER_BOOLSEARCH_EXAMPLE

#ifdef BIGGER_OPTIMISED_BOOLSEARCH_EXAMPLE

	std::chrono::time_point<std::chrono::system_clock> t1, t2;

	InvertedTextIndex<OptimisedPostingList> index;


	t1 = std::chrono::system_clock::now();

	generateRandomPostings(index, 3000, 3, 8, 10000, 3000);

	t2 = std::chrono::system_clock::now();

	std::cout << "\n" << " generating: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms;\nsize: " << index.sizeOfIndexInBytes() << " bytes";

	size_t numOfTerms = 0;
	size_t numOfDocids = 0;

	for (auto& term : index._termsPostingLists)
	{
		numOfTerms++;
		numOfDocids += term.second._docIdsInPosting;
	}

	std::cout << "\nnumber of terms: " << numOfTerms << "\nnumber of docids: " << numOfDocids << " (" << static_cast<double>(index.sizeOfIndexInBytes()) / numOfDocids << " bytes/docId)\n\n";

	size_t counter = 3;

	std::vector<std::pair<std::string, OptimisedPostingList>> somePostings;

	for (auto& term : index._termsPostingLists)
	{
		if (counter == 0)
			break;

		somePostings.push_back(term);

		std::cout << term.first << ": ";

		term.second.print();

		std::cout << "\n";

		counter--;
	}


	std::vector<std::string> termsForQuery;
	for (auto& term : somePostings)
	{
		termsForQuery.push_back(term.first);
	}

	auto andRes = index.queryAND(termsForQuery);

	std::cout << "\nAND (";

	for (auto& term : termsForQuery)
	{
		std::cout << "\"" << term << "\", ";
	}

	std::cout << "):\t";

	andRes.print();

#endif // BIGGER_OPTIMISED_BOOLSEARCH_EXAMPLE

#ifdef BIGGER_OPTIMISED_SKIP_BOOLSEARCH_EXAMPLE

	std::chrono::time_point<std::chrono::system_clock> t1, t2;

	InvertedTextIndex<OptimisedPostingListSkip> index;


	t1 = std::chrono::system_clock::now();

	generateRandomPostings(index, 3000, 3, 8, 10000, 3000);

	t2 = std::chrono::system_clock::now();

	std::cout << "\n" << " generating: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms;\nsize: " << index.sizeOfIndexInBytes() << " bytes";

	size_t numOfTerms = 0;
	size_t numOfDocids = 0;

	for (auto& term : index._termsPostingLists)
	{
		numOfTerms++;
		numOfDocids += term.second._docIdsInPosting;
	}

	std::cout << "\nnumber of terms: " << numOfTerms << "\nnumber of docids: " << numOfDocids << " (" << static_cast<double>(index.sizeOfIndexInBytes()) / numOfDocids << " bytes/docId)\n\n";

	size_t counter = 3;

	std::vector<std::pair<std::string, OptimisedPostingListSkip>> somePostings;

	for (auto& term : index._termsPostingLists)
	{
		if (counter == 0)
			break;

		somePostings.push_back(term);

		std::cout << term.first << ": ";

		term.second.print();

		std::cout << "\n";

		counter--;
	}


	std::vector<std::string> termsForQuery;
	for (auto& term : somePostings)
	{
		termsForQuery.push_back(term.first);
	}

	auto andRes = index.queryAND(termsForQuery);

	std::cout << "\nAND (";

	for (auto& term : termsForQuery)
	{
		std::cout << "\"" << term << "\", ";
	}

	std::cout << "):\t";

	andRes.print();

#endif // BIGGER_OPTIMISED_SKIP_BOOLSEARCH_EXAMPLE

#ifdef HUFFMAN_TEST

	HuffmanCodeProcessor<size_t> hp;

	std::vector<size_t> origValues;

	for (size_t i = 0; i < 128; i++)
	{
		origValues.push_back(rand() % 20);
	}

	for (size_t i = 0; i < origValues.size(); i++)
	{
		//std::cout << origValues[i] << ' ';
	}

	std::cout << '\n';

	hp.prepareCodeTreeAndTable(origValues);

	std::vector<uint8_t> encoded;

	hp.encodeHuffman(origValues, encoded);

	std::cout << "\n\n";
	for (size_t i = 0; i < encoded.size(); i++)
	{
		//std::cout << (int)encoded[i] << ' ';
	}
	std::cout << "\n\n";


	std::vector<size_t> decodedValues;

	hp.decodeHuffman(encoded, decodedValues, origValues.size());


	std::cout << '\n';

	for (size_t i = 0; i < decodedValues.size(); i++)
	{
		//std::cout << decodedValues[i] << ' ';
	}

	std::cout << "Equality of orig and decoded: " << (origValues == decodedValues) << "\noriginal size: " << origValues.size() * sizeof(size_t) << " bytes, encoded size: " << encoded.size() << " bytes";

	std::cout << '\n';

#endif // HUFFMAN_TEST

#ifdef SHOW_OPTIMISED_POSTING_ENCODING

	size_t maxDocId = 200;
	size_t postingListLen = 53;
	size_t docIdsInBlock = 10;

	std::vector<size_t> ids;
	ids.resize(maxDocId);

	for (size_t i = 0; i < ids.size(); i++)
		ids[i] = i + 1;

	std::random_shuffle(ids.begin(), ids.end());

	std::vector<size_t> postingList(ids.begin(), ids.begin() + postingListLen);
	std::sort(postingList.begin(), postingList.end());

	OptimisedPostingList pl(docIdsInBlock);
	pl.generateFromDocidsVector(postingList);

	for (size_t i = 0; i < pl._docIdsBlocks.size(); i++)
	{
		pl.decodeBlock(i);
	}

	std::cout << "\n\n";

	pl.print();


	std::cout << "\n\nsize of posting: " << pl.sizeInBytes() << " bytes;\tdocIds as is size: " << postingList.size() * sizeof(size_t) << " bytes\n\n";

	for (size_t i = 0; i < pl._docIdsInPosting + 5; i++)
	{
		std::cout << "cur: " << pl.current();
		std::cout << ", next: " << pl.next() << ";\t";
	}

	pl.reset();

	std::cout << "\n\n";

	for (size_t i = 0; i < pl._docIdsInPosting + 5; i++)
	{
		std::cout << "cur: " << pl.current();
		std::cout << ", advance " << pl.current() + 1 << ": ";
		std::cout << pl.advance(pl.current() + 1) << ";\t";
	}

	std::vector<size_t> postingListDocIds;
	pl.saveDocIdsInVector(postingListDocIds);

	std::cout << "\n\n";

	for (size_t i = 0; i < postingListDocIds.size(); i++)
	{
		std::cout << " " << postingListDocIds[i];
	}



#endif // SHOW_OPTIMISED_POSTING_ENCODING

#ifdef SHOW_OPTIMISED_SKIP_POSTING_ENCODING

	size_t maxDocId = 200;
	size_t postingListLen = 53;
	size_t docIdsInBlock = 10;

	std::vector<size_t> ids;
	ids.resize(maxDocId);

	for (size_t i = 0; i < ids.size(); i++)
		ids[i] = i + 1;

	std::random_shuffle(ids.begin(), ids.end());

	std::vector<size_t> postingList(ids.begin(), ids.begin() + postingListLen);
	std::sort(postingList.begin(), postingList.end());

	OptimisedPostingListSkip pl(docIdsInBlock);
	pl.generateFromDocidsVector(postingList);

	auto ptr = pl._firstDocIdBlockPtr;
	while (bool(ptr) == true)
	{
		pl.decodeBlock(ptr);
		ptr = ptr.get()->_forwardLinks[0];
	}

	std::cout << "\n\n";

	pl.print();


	std::cout << "\n\nsize of posting: " << pl.sizeInBytes() << " bytes;\tdocIds as is size: " << postingList.size() * sizeof(size_t) << " bytes\n\n";

	for (size_t i = 0; i < pl._docIdsInPosting + 5; i++)
	{
		std::cout << "cur: " << pl.current();
		std::cout << ", next: " << pl.next() << ";\t";
	}

	pl.reset();

	std::cout << "\n\n";

	for (size_t i = 0; i < pl._docIdsInPosting + 5; i++)
	{
		std::cout << "cur: " << pl.current();
		std::cout << ", advance " << pl.current() + 1 << ": ";
		std::cout << pl.advance(pl.current() + 1) << ";\t";
	}

	std::vector<size_t> postingListDocIds;
	pl.saveDocIdsInVector(postingListDocIds);

	std::cout << "\n\n";

	for (size_t i = 0; i < postingListDocIds.size(); i++)
	{
		std::cout << " " << postingListDocIds[i];
	}



#endif // SHOW_OPTIMISED_SKIP_POSTING_ENCODING

#ifdef CORRECTNESS_TEST

	size_t numOfTermsInIndex = 300;
	size_t minTermLen = 3;
	size_t maxTermLen = 8;
	size_t maxDocId = 100000;
	size_t maxPostingListLength = 300000;


	std::chrono::time_point<std::chrono::system_clock> t1, t2;

	InvertedTextIndex<PostingList> index;
	InvertedTextIndex<OptimisedPostingList> indexOpt;
	InvertedTextIndex<OptimisedPostingListSkip> indexOptSkip;

	///////////////////////////////////////////////////////////////////
	srand(0);
	t1 = std::chrono::system_clock::now();

	generateRandomPostings(index, numOfTermsInIndex, minTermLen, maxTermLen, maxDocId, maxPostingListLength);

	t2 = std::chrono::system_clock::now();

	std::cout << "\n" << " generating index <PostingList>: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms;\nsize: " << index.sizeOfIndexInBytes() << " bytes";

	size_t numOfTerms = 0;
	size_t numOfDocids = 0;

	for (auto& term : index._termsPostingLists)
	{
		numOfTerms++;
		numOfDocids += term.second._docIds.size();
	}

	std::cout << "\nnumber of terms: " << numOfTerms << "\nnumber of docids: " << numOfDocids << " (" << static_cast<double>(index.sizeOfIndexInBytes()) / numOfDocids << " bytes/docId)\n\n";

	///////////////////////////////////////////////////////////////////
	srand(0);
	t1 = std::chrono::system_clock::now();

	generateRandomPostings(indexOpt, numOfTermsInIndex, minTermLen, maxTermLen, maxDocId, maxPostingListLength);

	t2 = std::chrono::system_clock::now();

	std::cout << "\n" << " generating index <OptimisedPostingList>: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms;\nsize: " << indexOpt.sizeOfIndexInBytes() << " bytes";

	numOfTerms = 0;
	numOfDocids = 0;

	for (auto& term : indexOpt._termsPostingLists)
	{
		numOfTerms++;
		numOfDocids += term.second._docIdsInPosting;
	}

	std::cout << "\nnumber of terms: " << numOfTerms << "\nnumber of docids: " << numOfDocids << " (" << static_cast<double>(indexOpt.sizeOfIndexInBytes()) / numOfDocids << " bytes/docId)\n\n";

	///////////////////////////////////////////////////////////////////

	srand(0);
	t1 = std::chrono::system_clock::now();

	generateRandomPostings(indexOptSkip, numOfTermsInIndex, minTermLen, maxTermLen, maxDocId, maxPostingListLength);

	t2 = std::chrono::system_clock::now();

	std::cout << "\n" << " generating index <OptimisedPostingListSkip>: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms;\nsize: " << indexOpt.sizeOfIndexInBytes() << " bytes";

	numOfTerms = 0;
	numOfDocids = 0;

	for (auto& term : indexOpt._termsPostingLists)
	{
		numOfTerms++;
		numOfDocids += term.second._docIdsInPosting;
	}

	std::cout << "\nnumber of terms: " << numOfTerms << "\nnumber of docids: " << numOfDocids << " (" << static_cast<double>(indexOptSkip.sizeOfIndexInBytes()) / numOfDocids << " bytes/docId)\n\n";

	///////////////////////////////////////////////////////////////////

	size_t counter = 3;

	std::vector<std::pair<std::string, PostingList>> somePostings;

	for (auto& term : index._termsPostingLists)
	{
		if (counter == 0)
			break;

		somePostings.push_back(term);

		std::cout << term.first << ": ";

		//term.second.print();

		std::cout << "\n";
		counter--;
	}


	std::vector<std::string> termsForQuery;
	for (auto& term : somePostings)
	{
		termsForQuery.push_back(term.first);
	}

	t1 = std::chrono::system_clock::now();

	auto andRes = index.queryAND(termsForQuery);

	t2 = std::chrono::system_clock::now();

	std::cout << "\nAND <PostingList>: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " mcs; ";

	std::cout << "\nAND (";

	for (auto& term : termsForQuery)
	{
		std::cout << "\"" << term << "\", ";
	}

	std::cout << "):\t";

	//andRes.print();

	std::cout << "\n\n";

	///////////////////////////////////////////////////////////////////

	counter = 3;

	std::vector<std::pair<std::string, OptimisedPostingList>> somePostingsOpt;

	for (auto& term : indexOpt._termsPostingLists)
	{
		if (counter == 0)
			break;

		somePostingsOpt.push_back(term);

		std::cout << term.first << ": ";

		//term.second.print();

		std::cout << "\n";
		counter--;
	}


	std::vector<std::string> termsForQueryOpt;
	for (auto& term : somePostingsOpt)
	{
		termsForQueryOpt.push_back(term.first);
	}

	t1 = std::chrono::system_clock::now();

	auto andResOpt = indexOpt.queryAND(termsForQueryOpt);

	t2 = std::chrono::system_clock::now();

	std::cout << "\nAND <OptimisedPostingList>: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " mcs; ";

	std::cout << "\nAND (";

	for (auto& term : termsForQueryOpt)
	{
		std::cout << "\"" << term << "\", ";
	}

	std::cout << "):\t";

	//andResOpt.print();

	std::cout << "\n\n";

	///////////////////////////////////////////////////////////////////

	counter = 3;

	std::vector<std::pair<std::string, OptimisedPostingListSkip>> somePostingsOptSkip;

	for (auto& term : indexOptSkip._termsPostingLists)
	{
		if (counter == 0)
			break;

		somePostingsOptSkip.push_back(term);

		std::cout << term.first << ": ";

		//term.second.print();

		std::cout << "\n";
		counter--;
	}


	std::vector<std::string> termsForQueryOptSkip;
	for (auto& term : somePostingsOptSkip)
	{
		termsForQueryOptSkip.push_back(term.first);
	}

	t1 = std::chrono::system_clock::now();

	auto andResOptSkip = indexOptSkip.queryAND(termsForQueryOptSkip);

	t2 = std::chrono::system_clock::now();

	std::cout << "\nAND <OptimisedPostingListSkip>: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " mcs; ";

	std::cout << "\nAND (";

	for (auto& term : termsForQueryOptSkip)
	{
		std::cout << "\"" << term << "\", ";
	}

	std::cout << "):\t";

	//andResOpt.print();

	std::cout << "\n\n";

	///////////////////////////////////////////////////////////////////

	std::vector<size_t> andResDocIds;
	std::vector<size_t> andResOptDocIds;
	std::vector<size_t> andResOptSkipDocIds;

	andRes.saveDocIdsInVector(andResDocIds);
	andResOpt.saveDocIdsInVector(andResOptDocIds);
	andResOptSkip.saveDocIdsInVector(andResOptSkipDocIds);

	std::cout << "\n\nAND results equality " << ((andResDocIds == andResOptDocIds) && (andResDocIds == andResOptSkipDocIds)) << "\n\n";

	///////////////////////////////////////////////////////////////////

	t1 = std::chrono::system_clock::now();

	auto orRes = index.queryOR(termsForQuery);

	t2 = std::chrono::system_clock::now();

	std::cout << "\nOR <PostingList>: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " mcs; ";

	std::cout << "\nOR (";

	for (auto& term : termsForQuery)
	{
		std::cout << "\"" << term << "\", ";
	}

	std::cout << "):\t";

	//orRes.print();

	std::cout << "\n\n";

	///////////////////////////////////////////////////////////////////

	t1 = std::chrono::system_clock::now();

	auto orResOpt = indexOpt.queryOR(termsForQueryOpt);

	t2 = std::chrono::system_clock::now();

	std::cout << "\nOR <OptimisedPostingList>: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " mcs; ";

	std::cout << "\nOR (";

	for (auto& term : termsForQueryOpt)
	{
		std::cout << "\"" << term << "\", ";
	}

	std::cout << "):\t";

	//orResOpt.print();

	std::cout << "\n\n";

	///////////////////////////////////////////////////////////////////

	t1 = std::chrono::system_clock::now();

	auto orResOptSkip = indexOptSkip.queryOR(termsForQueryOptSkip);

	t2 = std::chrono::system_clock::now();

	std::cout << "\nOR <OptimisedPostingListSkip>: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " mcs; ";

	std::cout << "\nOR (";

	for (auto& term : termsForQueryOptSkip)
	{
		std::cout << "\"" << term << "\", ";
	}

	std::cout << "):\t";

	//orResOpt.print();

	std::cout << "\n\n";

	///////////////////////////////////////////////////////////////////

	std::vector<size_t> orResDocIds;
	std::vector<size_t> orResOptDocIds;
	std::vector<size_t> orResOptSkipDocIds;

	orRes.saveDocIdsInVector(orResDocIds);
	orResOpt.saveDocIdsInVector(orResOptDocIds);
	orResOptSkip.saveDocIdsInVector(orResOptSkipDocIds);

	std::cout << "\n\nOR results equality " << ((orResDocIds == orResOptDocIds) && (orResDocIds == orResOptSkipDocIds)) << "\n\n";


#endif // CORRECTNESS_TEST

#ifdef SHOW_SKIP_LIST_IN_DEBUGGER

	// watch in debugger (breakpoint on "std::cout << 0;")

	size_t radix = 6;

	size_t maxDocId = 200;
	size_t postingListLen = 53;

	std::vector<size_t> ids;
	ids.resize(maxDocId);

	for (size_t i = 0; i < ids.size(); i++)
		ids[i] = i + 1;

	std::random_shuffle(ids.begin(), ids.end());

	std::vector<size_t> postingList(ids.begin(), ids.begin() + postingListLen);
	std::sort(postingList.begin(), postingList.end());


	std::vector<std::shared_ptr<ScipListNode<size_t>>> scipListNodesPointers;
	scipListNodesPointers.resize(postingList.size());

//	scipListNodesPointers.assign(postingList.size(), std::make_shared<ScipListNode<size_t>>());
	for (size_t i = 0; i < scipListNodesPointers.size(); i++)
		scipListNodesPointers[i] = std::make_shared<ScipListNode<size_t>>();
	

	size_t logOfLenMinusOneFloor = 0;
	size_t postingListLenMinusOne = postingListLen - 1;

	while (postingListLenMinusOne >= radix)
	{
		postingListLenMinusOne = postingListLenMinusOne / radix;
		logOfLenMinusOneFloor++;
	}
	
	

	for (size_t i = 0; i < postingList.size(); i++)
	{
		scipListNodesPointers[i].get()->_value = postingList[i];
		scipListNodesPointers[i].get()->_forwardLinks.assign(logOfLenMinusOneFloor + 1, std::shared_ptr<ScipListNode<size_t>>());

		size_t radixPow = 1;
		for (size_t j = 0; j < logOfLenMinusOneFloor + 1; j++)
		{
			if (i + radixPow < postingList.size())
				scipListNodesPointers[i].get()->_forwardLinks[j] = scipListNodesPointers[i + radixPow];
			
			radixPow *= radix;
		}
	}

	std::cout << 0;

#endif // SHOW_SKIP_LIST_IN_DEBUGGER


	/*std::chrono::time_point<std::chrono::system_clock> t1, t2;


	std::vector<size_t> ids;
	ids.resize(1'000'000);

	for (size_t i = 0; i < ids.size(); i++)
	{
		ids[i] = i + 1;
	}

	std::vector<size_t> searchingIds;
	searchingIds.resize(100);

	for (size_t i = 0; i < searchingIds.size(); i++)
	{
		searchingIds[i] = rand() % ids.size();
	}

	size_t res = 0;

	t1 = std::chrono::system_clock::now();

	for (size_t i = 0; i < searchingIds.size(); i++)
	{
		res += (std::upper_bound(ids.begin(), ids.end(), ids[searchingIds[i]]) == ids.end());
	}

	t2 = std::chrono::system_clock::now();

	std::cout << "\n"<< res <<" upper_bound: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << " ns;";

	res = 0;


	t1 = std::chrono::system_clock::now();

	for (size_t i = 0; i < searchingIds.size(); i++)
	{
		res += left_bin_search(&(ids[0]), 0, ids.size() - 1, ids[searchingIds[i]]);
	}

	t2 = std::chrono::system_clock::now();

	std::cout << "\n" << res << " left_bin_search: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << " ns;";

	res = 0;

	t1 = std::chrono::system_clock::now();


	for (size_t j = 0; j < searchingIds.size(); j++)
	{
		for (size_t i = 0; i < ids.size(); i++)
		{
			if (ids[i] == ids[searchingIds[j]])
				res = i;
		}
	}

	t2 = std::chrono::system_clock::now();

	std::cout << "\n" << res << "linear: " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << " ns;";*/

}

