#pragma once

#include <cstdint>

class Image {
public:
	enum class ColorMode {
		Linear, sRGB
	};

private:
	enum class FileType {
		PNG, JPG, BMP, TGA
	};

public:
	Image();
	Image(const char* file, Image::ColorMode colorMode = Image::ColorMode::Linear);
	Image(const Image& copyImage);
	Image(const int w, const int h, const int channels);
	~Image();

	Image& operator=(const Image& copyImage);

public: // ------ MAIN FUNCTIONS -----
	bool Read(const char* file, Image::ColorMode colorMode = Image::ColorMode::Linear);
	bool Write(const char* file, Image::ColorMode colorMode = Image::ColorMode::Linear);

public: // ----- OTHER -----
	void BiLerp(const float x, const float y, float& r, float& g, float& b);
	void ToLinearRGB();
	void TosRGB();

public: // ----- GETTERS & SETTERS -----
	int GetIndex(const int x, const int y);

	int GetWidth() const { return m_w; };
	int GetHeight() const { return m_h; };
	int GetChannels() const { return m_channels; };

	size_t GetSize() { return m_size; };

	//void SetData(const int index, const float data) { m_dataF[index] = data; };
	//float GetData(const int index) { return m_dataF[index]; };

	void GetRGB(const int x, const int y, float& r, float& g, float& b);
	void SetRGB(const int x, const int y, const float r, const float g, const float b);

	static bool PrintToConsole;

private: // ----- MEMBER VARIABLE -----
	size_t m_size;
	int m_w;
	int m_h;
	int m_channels;

	// ----- DATA -----
	uint8_t* m_data;
	float* m_dataF;

private:
	Image::FileType GetFileType(const char* file);
	void ModulusUV(const int x, const int y, int& u, int& v);
};
