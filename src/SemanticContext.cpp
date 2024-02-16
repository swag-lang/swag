#include "pch.h"
#include "SemanticContext.h"

void SemanticContext::release()
{
	clearTryMatch();
	clearGenericMatch();
	clearMatch();

	for (const auto p : cacheFreeTryMatch)
		Allocator::free<OneTryMatch>(p);
	for (const auto p : cacheFreeGenericMatches)
		Allocator::free<OneMatch>(p);
	for (const auto p : cacheFreeMatches)
		Allocator::free<OneMatch>(p);
	tmpConcat.release();
	if (tmpIdRef)
		tmpIdRef->release();
}

void SemanticContext::clearTryMatch()
{
	for (auto p : cacheListTryMatch)
		cacheFreeTryMatch.push_back(p);
	cacheListTryMatch.clear();
}

OneTryMatch* SemanticContext::getTryMatch()
{
	if (cacheFreeTryMatch.empty())
		return Allocator::alloc<OneTryMatch>();
	const auto res = cacheFreeTryMatch.back();
	cacheFreeTryMatch.pop_back();
	res->reset();
	return res;
}

void SemanticContext::clearMatch()
{
	for (auto p : cacheMatches)
		cacheFreeMatches.push_back(p);
	cacheMatches.clear();
}

OneMatch* SemanticContext::getOneMatch()
{
	if (cacheFreeMatches.empty())
		return Allocator::alloc<OneMatch>();
	const auto res = cacheFreeMatches.back();
	cacheFreeMatches.pop_back();
	res->reset();
	return res;
}

void SemanticContext::clearGenericMatch()
{
	for (auto p : cacheGenericMatches)
		cacheFreeGenericMatches.push_back(p);
	cacheGenericMatches.clear();
	cacheGenericMatchesSI.clear();
}

OneMatch* SemanticContext::getOneGenericMatch()
{
	if (cacheFreeGenericMatches.empty())
		return Allocator::alloc<OneMatch>();
	const auto res = cacheFreeGenericMatches.back();
	cacheFreeGenericMatches.pop_back();
	res->reset();
	return res;
}
