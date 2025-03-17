#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
#include <set>
#include <memory>
#include "inverted_text_index.h"

template class InvertedTextIndex<PostingList>;
template class InvertedTextIndex<OptimisedPostingList>;
template class InvertedTextIndex<OptimisedPostingListSkip>;

template void generateRandomPostings(InvertedTextIndex<PostingList> &index, size_t numOfTerms, size_t minTermLen, size_t maxTermLen, size_t maxDocId, size_t maxPostingListLength);
template void generateRandomPostings(InvertedTextIndex<OptimisedPostingList> &index, size_t numOfTerms, size_t minTermLen, size_t maxTermLen, size_t maxDocId, size_t maxPostingListLength);
template void generateRandomPostings(InvertedTextIndex<OptimisedPostingListSkip> &index, size_t numOfTerms, size_t minTermLen, size_t maxTermLen, size_t maxDocId, size_t maxPostingListLength);
// TO DO:

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

	for (auto &term : terms)
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

	for (auto &posting : postingLists)
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

	for (auto &term : terms)
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

	for (auto &posting : postingLists)
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
		for (auto &posting : postingLists)
		{
			if (posting.current() > currentMaxDocId)
				currentMaxDocId = posting.current();
		}

		for (auto &posting : postingLists)
			posting.advance(currentMaxDocId);

		bool allEqual = true;

		size_t valToCompare = postingLists[0].current();

		for (auto &posting : postingLists)
		{
			if (posting.current() == 0)
			{
				atLeastOneEndOfPostingListReached = true;
				break;
			}
		}

		for (auto &posting : postingLists)
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

	for (auto &term : terms)
	{
		auto iter = _termsPostingLists.find(term);

		if (iter != _termsPostingLists.end())
		{
			postingLists.push_back((*iter).second);
		}
	}

	for (auto &term : termsToDiscard)
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

	for (auto &term : terms)
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

	for (auto &term : termsToDiscard)
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

	for (auto &posting : postingLists)
	{
		posting.reset();

		while (posting.current() != 0)
		{
			postingListsDocIds.insert(posting.current());
			posting.next();
		}
	}

	std::set<size_t> postingListsToDiscardDocIds = {};

	for (auto &posting : postingListsToDiscard)
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
	for (auto &termPosting : _termsPostingLists)
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

	for (auto &termPosting : _termsPostingLists)
	{
		res += termPosting.first.length();
		res += termPosting.second.sizeInBytes();
	}

	return res;
}

template <class PostingListType>
void generateRandomPostings(InvertedTextIndex<PostingListType> &index, size_t numOfTerms, size_t minTermLen, size_t maxTermLen, size_t maxDocId, size_t maxPostingListLength)
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
