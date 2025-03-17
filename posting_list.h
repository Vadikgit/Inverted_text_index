#ifndef POSTING_LIST
#define POSTING_LIST

#include "huffman_code_processor.h"

// TO DO:

class PostingList
{
public:
	PostingList();
	~PostingList();

	std::vector<size_t> _docIds; // 1 .. smth    0 - endOfList, not stored, but can be returned

	size_t _currentPos;

	size_t next();
	size_t advance(size_t docId);
	size_t current();

	void reset();
	void generateFromDocidsVector(std::vector<size_t> &docIds);
	void print();
	size_t sizeInBytes();

	void saveDocIdsInVector(std::vector<size_t> &dst);
};
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

	OptimisedPostingList(size_t docIdsInBlock = 128);
	~OptimisedPostingList();

	size_t _currentPos;

	size_t next();
	size_t advance(size_t docId);
	size_t current();

	void reset();
	void generateFromDocidsVector(std::vector<size_t> &docIds);
	void print();
	size_t sizeInBytes();

	void decodeBlock(size_t blockNumber);

	void saveDocIdsInVector(std::vector<size_t> &dst);
};

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

	OptimisedPostingListSkip(size_t docIdsInBlock = 128);
	~OptimisedPostingListSkip();

	size_t _currentPos;

	size_t next();
	size_t advance(size_t docId);
	size_t current();

	void reset();
	void generateFromDocidsVector(std::vector<size_t> &docIds);
	void print();
	size_t sizeInBytes();

	void decodeBlock(std::shared_ptr<ScipListNode<DocIdsBlock>> blockPtr);

	void saveDocIdsInVector(std::vector<size_t> &dst);
};

#endif