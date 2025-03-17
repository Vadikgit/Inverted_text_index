#ifndef INVERTED_TEXT_INDEX
#define INVERTED_TEXT_INDEX

#include <unordered_map>
#include <string>
#include <vector>
#include "posting_list.h"

// TO DO:

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
void generateRandomPostings(InvertedTextIndex<PostingListType> &index, size_t numOfTerms, size_t minTermLen, size_t maxTermLen, size_t maxDocId, size_t maxPostingListLength);

#endif