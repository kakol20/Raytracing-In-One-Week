#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>
#include <cstdio>
#include <string.h>
#include <iostream>

enum class ImageType {
	PNG, JPG, BMP, TGA
};

class Image {
public:
	Image();
	Image(const char* fileName);
	Image(int w, int h, int channels);

	Image(const Image& copyImage);
	Image operator=(const Image& copyImage);

	bool Read(const char* fileName);
	bool Write(const char* fileName);

	int GetRow();
	int GetIndex(int x, int y);

	int GetWidth() { return m_w; };
	int GetHeight() { return m_h; };
	int GetChannels() { return m_channels; };

	size_t GetSize();

	uint8_t GetData(int index);
	void SetData(int index, uint8_t data);

	float GetDataF(int index);
	void SetData(int index, float data);

	void BackgroundColor(const float r, float g, float b, float a);

	~Image();

public: // static public
	static bool PrintToConsole;

private:

	uint8_t* m_data = NULL;
	float* m_dataF = NULL;

	size_t m_size = 0;
	int m_w;
	int m_h;
	int m_channels;

	ImageType GetFileType(const char* fileName);
};

#endif // !IMAGE_H
