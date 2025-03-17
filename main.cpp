#include <iostream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <chrono>
#include <set>
#include <queue>
#include <unordered_set>
#include <memory>
#include "inverted_text_index.h"

// #define SIMPLE_BOOLSEARCH_EXAMPLE
// #define SIMPLE_OPTIMISED_BOOLSEARCH_EXAMPLE
// #define SIMPLE_OPTIMISED_SKIP_BOOLSEARCH_EXAMPLE

// #define BIGGER_BOOLSEARCH_EXAMPLE
// #define BIGGER_OPTIMISED_BOOLSEARCH_EXAMPLE
// #define BIGGER_OPTIMISED_SKIP_BOOLSEARCH_EXAMPLE
// #define HUFFMAN_TEST
// #define SHOW_OPTIMISED_POSTING_ENCODING
// #define SHOW_OPTIMISED_SKIP_POSTING_ENCODING
// #define SHOW_SKIP_LIST_IN_DEBUGGER

#define CORRECTNESS_TEST
// #define PERFORMANCE_TEST

// #define SHOW_DECODED_BLOCKS_EVERY_TIME
// #define SHOW_HUFFMAN_PROCESS

// TO DO:

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

	auto orRes = index.queryOR({"buSB", "ooeFx", "mnM"});

	std::cout << "\nOR (\"buSB\", \"ooeFx\", \"mnM\"):\t";

	orRes.print();
	std::cout << "\n";

	auto andRes = index.queryAND(std::vector<std::string>{"buSB", "ooeFx", "uGgGe"});

	std::cout << "\nAND (\"buSB\", \"ooeFx\", \"uGgGe\"):\t";

	andRes.print();
	std::cout << "\n";

	auto notRes = index.filterNO(std::vector<PostingList>{orRes}, std::vector<std::string>{"buSB", "ooeFx"});

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

	auto orRes = index.queryOR({"buSB", "ooeFx", "mnM"});

	std::cout << "\nOR (\"buSB\", \"ooeFx\", \"mnM\"):\t";

	orRes.print();
	std::cout << "\n";

	auto andRes = index.queryAND(std::vector<std::string>{"buSB", "ooeFx", "uGgGe"});

	std::cout << "\nAND (\"buSB\", \"ooeFx\", \"uGgGe\"):\t";

	andRes.print();
	std::cout << "\n";

	auto notRes = index.filterNO(std::vector<OptimisedPostingList>{orRes}, std::vector<std::string>{"buSB", "ooeFx"});

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

	auto orRes = index.queryOR({"buSB", "ooeFx", "mnM"});

	std::cout << "\nOR (\"buSB\", \"ooeFx\", \"mnM\"):\t";

	orRes.print();
	std::cout << "\n";

	auto andRes = index.queryAND(std::vector<std::string>{"buSB", "ooeFx", "uGgGe"});

	std::cout << "\nAND (\"buSB\", \"ooeFx\", \"uGgGe\"):\t";

	andRes.print();
	std::cout << "\n";

	auto notRes = index.filterNO(std::vector<OptimisedPostingListSkip>{orRes}, std::vector<std::string>{"buSB", "ooeFx"});

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

	std::cout << "\n"
			  << " generating: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms;\nsize: " << index.sizeOfIndexInBytes() << " bytes";

	size_t numOfTerms = 0;
	size_t numOfDocids = 0;

	for (auto &term : index._termsPostingLists)
	{
		numOfTerms++;
		numOfDocids += term.second._docIds.size();
	}

	std::cout << "\nnumber of terms: " << numOfTerms << "\nnumber of docids: " << numOfDocids << " (" << static_cast<double>(index.sizeOfIndexInBytes()) / numOfDocids << " bytes/docId)\n\n";

	size_t counter = 3;

	std::vector<std::pair<std::string, PostingList>> somePostings;

	for (auto &term : index._termsPostingLists)
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
	for (auto &term : somePostings)
	{
		termsForQuery.push_back(term.first);
	}

	auto andRes = index.queryAND(termsForQuery);

	std::cout << "\nAND (";

	for (auto &term : termsForQuery)
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

	std::cout << "\n"
			  << " generating: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms;\nsize: " << index.sizeOfIndexInBytes() << " bytes";

	size_t numOfTerms = 0;
	size_t numOfDocids = 0;

	for (auto &term : index._termsPostingLists)
	{
		numOfTerms++;
		numOfDocids += term.second._docIdsInPosting;
	}

	std::cout << "\nnumber of terms: " << numOfTerms << "\nnumber of docids: " << numOfDocids << " (" << static_cast<double>(index.sizeOfIndexInBytes()) / numOfDocids << " bytes/docId)\n\n";

	size_t counter = 3;

	std::vector<std::pair<std::string, OptimisedPostingList>> somePostings;

	for (auto &term : index._termsPostingLists)
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
	for (auto &term : somePostings)
	{
		termsForQuery.push_back(term.first);
	}

	auto andRes = index.queryAND(termsForQuery);

	std::cout << "\nAND (";

	for (auto &term : termsForQuery)
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

	std::cout << "\n"
			  << " generating: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms;\nsize: " << index.sizeOfIndexInBytes() << " bytes";

	size_t numOfTerms = 0;
	size_t numOfDocids = 0;

	for (auto &term : index._termsPostingLists)
	{
		numOfTerms++;
		numOfDocids += term.second._docIdsInPosting;
	}

	std::cout << "\nnumber of terms: " << numOfTerms << "\nnumber of docids: " << numOfDocids << " (" << static_cast<double>(index.sizeOfIndexInBytes()) / numOfDocids << " bytes/docId)\n\n";

	size_t counter = 3;

	std::vector<std::pair<std::string, OptimisedPostingListSkip>> somePostings;

	for (auto &term : index._termsPostingLists)
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
	for (auto &term : somePostings)
	{
		termsForQuery.push_back(term.first);
	}

	auto andRes = index.queryAND(termsForQuery);

	std::cout << "\nAND (";

	for (auto &term : termsForQuery)
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
		// std::cout << origValues[i] << ' ';
	}

	std::cout << '\n';

	hp.prepareCodeTreeAndTable(origValues);

	std::vector<uint8_t> encoded;

	hp.encodeHuffman(origValues, encoded);

	std::cout << "\n\n";
	for (size_t i = 0; i < encoded.size(); i++)
	{
		// std::cout << (int)encoded[i] << ' ';
	}
	std::cout << "\n\n";

	std::vector<size_t> decodedValues;

	hp.decodeHuffman(encoded, decodedValues, origValues.size());

	std::cout << '\n';

	for (size_t i = 0; i < decodedValues.size(); i++)
	{
		// std::cout << decodedValues[i] << ' ';
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

	std::cout << "\n"
			  << " generating index <PostingList>: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms;\nsize: " << index.sizeOfIndexInBytes() << " bytes";

	size_t numOfTerms = 0;
	size_t numOfDocids = 0;

	for (auto &term : index._termsPostingLists)
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

	std::cout << "\n"
			  << " generating index <OptimisedPostingList>: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms;\nsize: " << indexOpt.sizeOfIndexInBytes() << " bytes";

	numOfTerms = 0;
	numOfDocids = 0;

	for (auto &term : indexOpt._termsPostingLists)
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

	std::cout << "\n"
			  << " generating index <OptimisedPostingListSkip>: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms;\nsize: " << indexOpt.sizeOfIndexInBytes() << " bytes";

	numOfTerms = 0;
	numOfDocids = 0;

	for (auto &term : indexOpt._termsPostingLists)
	{
		numOfTerms++;
		numOfDocids += term.second._docIdsInPosting;
	}

	std::cout << "\nnumber of terms: " << numOfTerms << "\nnumber of docids: " << numOfDocids << " (" << static_cast<double>(indexOptSkip.sizeOfIndexInBytes()) / numOfDocids << " bytes/docId)\n\n";

	///////////////////////////////////////////////////////////////////

	size_t counter = 3;

	std::vector<std::pair<std::string, PostingList>> somePostings;

	for (auto &term : index._termsPostingLists)
	{
		if (counter == 0)
			break;

		somePostings.push_back(term);

		std::cout << term.first << ": ";

		// term.second.print();

		std::cout << "\n";
		counter--;
	}

	std::vector<std::string> termsForQuery;
	for (auto &term : somePostings)
	{
		termsForQuery.push_back(term.first);
	}

	t1 = std::chrono::system_clock::now();

	auto andRes = index.queryAND(termsForQuery);

	t2 = std::chrono::system_clock::now();

	std::cout << "\nAND <PostingList>: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " mcs; ";

	std::cout << "\nAND (";

	for (auto &term : termsForQuery)
	{
		std::cout << "\"" << term << "\", ";
	}

	std::cout << "):\t";

	// andRes.print();

	std::cout << "\n\n";

	///////////////////////////////////////////////////////////////////

	counter = 3;

	std::vector<std::pair<std::string, OptimisedPostingList>> somePostingsOpt;

	for (auto &term : indexOpt._termsPostingLists)
	{
		if (counter == 0)
			break;

		somePostingsOpt.push_back(term);

		std::cout << term.first << ": ";

		// term.second.print();

		std::cout << "\n";
		counter--;
	}

	std::vector<std::string> termsForQueryOpt;
	for (auto &term : somePostingsOpt)
	{
		termsForQueryOpt.push_back(term.first);
	}

	t1 = std::chrono::system_clock::now();

	auto andResOpt = indexOpt.queryAND(termsForQueryOpt);

	t2 = std::chrono::system_clock::now();

	std::cout << "\nAND <OptimisedPostingList>: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " mcs; ";

	std::cout << "\nAND (";

	for (auto &term : termsForQueryOpt)
	{
		std::cout << "\"" << term << "\", ";
	}

	std::cout << "):\t";

	// andResOpt.print();

	std::cout << "\n\n";

	///////////////////////////////////////////////////////////////////

	counter = 3;

	std::vector<std::pair<std::string, OptimisedPostingListSkip>> somePostingsOptSkip;

	for (auto &term : indexOptSkip._termsPostingLists)
	{
		if (counter == 0)
			break;

		somePostingsOptSkip.push_back(term);

		std::cout << term.first << ": ";

		// term.second.print();

		std::cout << "\n";
		counter--;
	}

	std::vector<std::string> termsForQueryOptSkip;
	for (auto &term : somePostingsOptSkip)
	{
		termsForQueryOptSkip.push_back(term.first);
	}

	t1 = std::chrono::system_clock::now();

	auto andResOptSkip = indexOptSkip.queryAND(termsForQueryOptSkip);

	t2 = std::chrono::system_clock::now();

	std::cout << "\nAND <OptimisedPostingListSkip>: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " mcs; ";

	std::cout << "\nAND (";

	for (auto &term : termsForQueryOptSkip)
	{
		std::cout << "\"" << term << "\", ";
	}

	std::cout << "):\t";

	// andResOpt.print();

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

	for (auto &term : termsForQuery)
	{
		std::cout << "\"" << term << "\", ";
	}

	std::cout << "):\t";

	// orRes.print();

	std::cout << "\n\n";

	///////////////////////////////////////////////////////////////////

	t1 = std::chrono::system_clock::now();

	auto orResOpt = indexOpt.queryOR(termsForQueryOpt);

	t2 = std::chrono::system_clock::now();

	std::cout << "\nOR <OptimisedPostingList>: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " mcs; ";

	std::cout << "\nOR (";

	for (auto &term : termsForQueryOpt)
	{
		std::cout << "\"" << term << "\", ";
	}

	std::cout << "):\t";

	// orResOpt.print();

	std::cout << "\n\n";

	///////////////////////////////////////////////////////////////////

	t1 = std::chrono::system_clock::now();

	auto orResOptSkip = indexOptSkip.queryOR(termsForQueryOptSkip);

	t2 = std::chrono::system_clock::now();

	std::cout << "\nOR <OptimisedPostingListSkip>: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " mcs; ";

	std::cout << "\nOR (";

	for (auto &term : termsForQueryOptSkip)
	{
		std::cout << "\"" << term << "\", ";
	}

	std::cout << "):\t";

	// orResOpt.print();

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
