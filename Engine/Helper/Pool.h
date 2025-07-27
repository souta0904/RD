#pragma once
#include <functional>
#include <memory>
#include <vector>

// TODO: リングバッファを実装

// アイテムプール
template <typename T>
class Pool
{
private:
	struct Item
	{
		T mVal;
		bool mIsActive = false;
	};

public:
	Pool(uint32_t size)
		: mItems(size)
	{
		for (uint32_t i = 0; i < size; ++i)
		{
			mItems[i] = std::make_unique<Item>();
		}
	}

	// アイテム確保
	T* Alloc(std::function<void(uint32_t, T&)> initFunc = nullptr)
	{
		for (uint32_t i = 0; i < mItems.size(); ++i)
		{
			if (!mItems[i]->mIsActive)
			{
				mItems[i]->mIsActive = true;
				if (initFunc)
				{
					initFunc(i, mItems[i]->mVal);
				}
				return &mItems[i]->mVal;
			}
		}
		return nullptr;
	}

	// アイテム解放
	void Free(T* val)
	{
		for (auto& item : mItems)
		{
			if (&item->mVal == val)
			{
				item->mIsActive = false;
				break;
			}
		}
	}

	uint32_t GetSize() const
	{
		return static_cast<uint32_t>(mItems.size());
	}

	uint32_t GetActiveCount() const
	{
		uint32_t counter = 0;
		for (const auto& item : mItems)
		{
			if (item->mIsActive)
			{
				++counter;
			}
		}
		return counter;
	}

	uint32_t GetInactiveCount() const
	{
		return GetSize() - GetActiveCount();
	}

private:
	std::vector<std::unique_ptr<Item>> mItems;
};
