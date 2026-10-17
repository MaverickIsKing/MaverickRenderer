#pragma once
#include <vector>
#include "Common.h"

inline int clampPadding(int x, int max)
{
	return glm::clamp(x, 0, max - 1);
}


inline int warpPadding(int x, int max)
{
	x = x % max;
	if (x < 0)
	{
		x += max;
	}
	return x;
}

inline int mirrorPadding(int x, int max)
{
	int sign = (x / max) % 2;
	x = x % max;
	if (x < 0)
	{
		x += max;
	}
	if (sign != 0)
	{
		x = max - x - 1;
	}
	return x;
}

class Texture
{
public:
	Texture(int width, int height, int channels, uint8_t* data) : mWidth(width), mHeight(height), mChannels(channels)
	{
		mData.resize(width * height * channels);
		memcpy(mData.data(), data, width * height * channels);
	}

	static Texture* CreateFromFile(const std::string& filepath);

	template <typename T>
	T GetPixel(int x, int y)
	{	
		
		x = mirrorPadding(x, mWidth);
		y = mirrorPadding(y, mHeight);

		int index = (y * mWidth + x) * mChannels;
		
		if constexpr (std::is_same_v<T, float>)
		{
			return mData[index] / 255.0f;
		}
		else
		{
			T result(0);
			int count = std::min(T::length(), mChannels);
			for (int i = 0; i < count; i++)
			{
				result[i] = mData[index + i] / 255.0f;
			}
			return result;
		}
	}

	template <typename T>
	T Sample(Vector2f uv)
	{	
		//point filtering
		//int x = std::round(uv.x * mWidth);
		//int y = std::round(uv.y * mHeight);
		
		//linear filtering
		float fx = uv.x * mWidth - 0.5f;
		float fy = uv.y * mHeight - 0.5f;

		int x = (int)std::floor(fx);
		int y = (int)std::floor(fy);

		float k1 = fx - x;
		float k2 = fy - y;

		T c1 = GetPixel<T>(x, y);
		T c2 = GetPixel<T>(x + 1, y);
		T c3 = GetPixel<T>(x, y + 1);
		T c4 = GetPixel<T>(x + 1, y + 1);
		T c5 = glm::mix(c1, c2, k1);
		T c6 = glm::mix(c3, c4, k1);
		return glm::mix(c5, c6, k2);

	}

private:
	int mWidth;
	int mHeight;
	int mChannels;
	std::vector<uint8_t> mData;
};