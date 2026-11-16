#pragma once
#include <vector>
class MemoryPool
{
public:

	static constexpr size_t ChunkSize = 4096;
	
	void* Allocate(size_t size)
	{
		if (mChunks.empty() || mCurrentPos + size > ChunkSize)
		{
			uint8_t* newChunk = (uint8_t*)std::malloc(ChunkSize);
			mChunks.push_back(newChunk);
			mCurrentPos = 0;
		}
		void* allocatedMemory = mChunks.back() + mCurrentPos;
		mCurrentPos += size;
		return allocatedMemory;
	}

	~MemoryPool()
	{
		for (uint8_t* pChunk : mChunks)
		{
			std::free(pChunk);
		}
	}

	static MemoryPool& GetInstance()
	{
		static MemoryPool instance;
		return instance;
	}
private:
	std::vector<uint8_t*> mChunks;
	int mCurrentPos = 0;
};