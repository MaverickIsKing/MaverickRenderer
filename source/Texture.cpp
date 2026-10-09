#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture* Texture::CreateFromFile(const std::string& filepath)
{
	int w, h, channels;

	uint8_t* data = stbi_load(filepath.c_str(), &w, &h, &channels, 0);
	if (data == nullptr)
	{
		return nullptr;
	}
	Texture* tex = new Texture(w, h, channels, data);
	stbi_image_free(data);
	return tex;
}