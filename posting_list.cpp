#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <queue>
#include <unordered_set>
#include <memory>
#include "posting_list.h"

const size_t skipListLinksProbogateRadix = 2;

// #define SHOW_OPTIMISED_POSTING_ENCODING
// #define SHOW_OPTIMISED_SKIP_POSTING_ENCODING
// #define SHOW_DECODED_BLOCKS_EVERY_TIME

// TO DO:

size_t left_bin_search(size_t *arr, size_t l, size_t r, size_t val)
{
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

//////////////////////////////////////////////////////////

PostingList::PostingList() : _currentPos{0} {};
PostingList::~PostingList() {};

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

void PostingList::generateFromDocidsVector(std::vector<size_t> &docIds)
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

void PostingList::saveDocIdsInVector(std::vector<size_t> &dst)
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

OptimisedPostingList::OptimisedPostingList(size_t docIdsInBlock) : _docIdsInBlock{docIdsInBlock}, _currentPos{0}, _numberOfCurrentDecodedBlock{-1} { _decodedCurrentBlockDocIds.assign(_docIdsInBlock, 0); };
OptimisedPostingList::~OptimisedPostingList() {};

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

	DocIdsBlock &block = _docIdsBlocks[blockNumber];

	size_t numberOfDecodedDocIds = std::min(_docIdsInBlock, _docIdsInPosting - _docIdsInBlock * (blockNumber));

	for (auto &i : _decodedCurrentBlockDocIds)
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

void OptimisedPostingList::generateFromDocidsVector(std::vector<size_t> &docIds)
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

	std::sort(deltas.begin(), deltas.end(), [&](size_t d1, size_t d2)
			  { return numberOfEachDelta[d1] < numberOfEachDelta[d2]; });

#ifdef SHOW_OPTIMISED_POSTING_ENCODING

	size_t _totalNumberOfDeltas = docIds.size() - ((docIds.size() + _docIdsInBlock - 1) / _docIdsInBlock);
	size_t _numberOfPassedDeltas = 0;
	bool _alreadyDone = false;

	std::cout << "\n\nDeltas from rarest to most frequent:\n";
	for (size_t i = 0; i < deltas.size(); i++)
	{
		_numberOfPassedDeltas += numberOfEachDelta[deltas[i]];
		std::cout << ' ' << deltas[i] << "\t(passed " << _numberOfPassedDeltas << " deltas = " << static_cast<double>(_numberOfPassedDeltas) / _totalNumberOfDeltas << ")\n";

		// if ((_numberOfPassedDeltas >= _totalNumberOfDeltas / 5) && (_alreadyDone == false))
		//{
		//	std::cout << "-----------------------------------\n";
		//	_alreadyDone = true;
		// }

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

		if ((i + 1) >= deltas.size() / 5)
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
		std::cout << "block " << i << " :\t";

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

	res += sizeof(_docIdsInBlock) + sizeof(_docIdsInPosting) + sizeof(encodingExceptionValue) + sizeof(_numberOfCurrentDecodedBlock) + _decodedCurrentBlockDocIds.size() * sizeof(size_t);

	for (size_t i = 0; i < _docIdsBlocks.size(); i++)
	{
		res += sizeof(_docIdsBlocks[i].firstDocIdInBlock) + _docIdsBlocks[i].encodedDocIds.size() + _docIdsBlocks[i].notEncodedDeltas.size() * sizeof(size_t);
	}

	for (auto &i : _huffmanCodeProcessor.Codes)
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

void OptimisedPostingList::saveDocIdsInVector(std::vector<size_t> &dst)
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

OptimisedPostingListSkip::OptimisedPostingListSkip(size_t docIdsInBlock) : _linksProbogateRadix{skipListLinksProbogateRadix}, _docIdsInBlock{docIdsInBlock}, _currentPos{0}
{
	_decodedCurrentBlockDocIds.assign(_docIdsInBlock, 0);
};
OptimisedPostingListSkip::~OptimisedPostingListSkip() {};

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
		while ((bool(ptr.get()->_forwardLinks[currentRadixPow]) == true) && (ptr.get()->_forwardLinks[currentRadixPow].get()->_value.firstDocIdInBlock <= docId))
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
	if (_currentDecodedBlockPtr == blockPtr) //?????/////
		return;

	if (_docIdsInPosting == 0)
		return;

	_currentDecodedBlockPtr = blockPtr;

	size_t numberOfDecodedDocIds = _docIdsInBlock;

	if ((bool(blockPtr.get()->_forwardLinks[0]) == false) && (_docIdsInPosting % _docIdsInBlock != 0)) // last block and it is not full
	{
		numberOfDecodedDocIds = _docIdsInPosting % _docIdsInBlock;
	}

	for (auto &i : _decodedCurrentBlockDocIds)
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

void OptimisedPostingListSkip::generateFromDocidsVector(std::vector<size_t> &docIds)
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

	std::sort(deltas.begin(), deltas.end(), [&](size_t d1, size_t d2)
			  { return numberOfEachDelta[d1] < numberOfEachDelta[d2]; });

#ifdef SHOW_OPTIMISED_SKIP_POSTING_ENCODING

	size_t _totalNumberOfDeltas = docIds.size() - ((docIds.size() + _docIdsInBlock - 1) / _docIdsInBlock);
	size_t _numberOfPassedDeltas = 0;
	bool _alreadyDone = false;

	std::cout << "\n\nDeltas from rarest to most frequent:\n";
	for (size_t i = 0; i < deltas.size(); i++)
	{
		_numberOfPassedDeltas += numberOfEachDelta[deltas[i]];
		std::cout << ' ' << deltas[i] << "\t(passed " << _numberOfPassedDeltas << " deltas = " << static_cast<double>(_numberOfPassedDeltas) / _totalNumberOfDeltas << ")\n";

		// if ((_numberOfPassedDeltas >= _totalNumberOfDeltas / 5) && (_alreadyDone == false))
		//{
		//	std::cout << "-----------------------------------\n";
		//	_alreadyDone = true;
		// }

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

	while (bool(ptr) == true)
	{
		res += sizeof(ptr.get()->_value.firstDocIdInBlock) + ptr.get()->_value.encodedDocIds.size() + ptr.get()->_value.notEncodedDeltas.size() * sizeof(size_t);
		res += ptr.get()->_forwardLinks.size() * sizeof(ptr.get()->_forwardLinks[0]);

		ptr = ptr.get()->_forwardLinks[0];
	}

	for (auto &i : _huffmanCodeProcessor.Codes)
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

void OptimisedPostingListSkip::saveDocIdsInVector(std::vector<size_t> &dst)
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
