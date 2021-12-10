#include "Filters.h"

const unsigned int Threshold8[] = {
	 0, 32,  8, 40,  2, 34, 10, 42,
	48, 16, 56, 24, 50, 18, 58, 26,
	12, 44,  4, 36, 14, 46,  6, 38,
	60, 28, 52, 20, 62, 30, 54, 22,
	 3, 35, 11, 43,  1, 33,  9, 41,
	51, 19, 59, 27, 49, 17, 57, 25,
	15, 47,  7, 39, 13, 45,  5, 37,
	63, 31, 55, 23, 61, 29, 53, 21
};

const unsigned int BlueNoise16[] = {
	111,  25, 125,  41,   2, 223,  83, 100, 232, 208, 108,  16, 246,  29, 216,  73,
	 49,  99, 178, 202, 244, 164, 196, 180,  31, 130, 188,  75, 175,  93, 233, 191,
	142, 239,  79, 152,  56, 114,  21, 145,  69, 243,  51, 219, 119, 150,  61,   9,
	162, 222,  15, 132,  91,  36, 254, 122,  11, 160,  89,  39, 200,  22, 128, 210,
	113,  32,  65, 189, 230, 214,  76, 172, 205, 224, 137,   0, 251, 166,  81,  43,
	195, 250, 173, 104,   5, 156,  45,  97,  58, 110, 186,  67, 103, 182, 237,  96,
	 71, 148, 123,  53, 204, 139, 179, 235,  18,  34, 154, 228, 146,  55, 217,   7,
	177,  19,  87, 236,  28,  70, 115, 129, 193, 248,  78, 121,  14,  30, 118, 136,
	201,  38, 213, 161, 187, 245,  12, 215,  88, 165,  47, 197, 209,  90, 159, 231,
	 50, 106, 131,  62, 101,  84,  40, 149,  60,  24, 134, 240, 174,  64, 255,  80,
	151, 220, 247,   1, 144, 226, 169, 199, 112, 234,  98,   3, 109,  42, 185,  10,
	 94, 170,  23, 181, 206,  48, 105,   8, 221, 184, 157,  74, 218, 141,  27, 124,
	 66, 194, 116,  77,  33, 126, 253,  72, 140,  52,  35, 127, 192, 168, 242, 225,
	 37, 138,  54, 241,  92, 158, 176,  26,  86, 198, 249,  20,  82,  57, 102, 207,
	 85,  13, 229, 147, 190,  17, 211, 238, 120, 171,  95, 227, 203, 117,   4, 155,
	252, 167, 212,  68, 107, 135,  59,  44, 153,   6,  63, 143, 163,  46, 133, 183,
};

float ClampF(float val, float low, float high)
{
	if (val > high) return high;
	if (val < low) return low;
	return val;
}

void Grayscale(Image & image)
{
	int w = image.GetWidth();
	int h = image.GetHeight();

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			int index = image.GetIndex(x, y);

			/*uint8_t r = image->GetData(index + 0);
			uint8_t g = image->GetData(index + 1);
			uint8_t b = image->GetData(index + 2);*/

			float r = image.GetDataF(index + 0);
			float g = image.GetDataF(index + 0);
			float b = image.GetDataF(index + 0);

			float grayscale = (r * 0.299f) + (g * 0.587f) + (b * 0.114f);

			image.SetData(index + 0, grayscale);
			image.SetData(index + 1, grayscale);
			image.SetData(index + 2, grayscale);
		}
	}
}

void OrderedDithering(Image & image, DitherFilter filter, Threshold thresholdMap, int factor)
{
	int w = image.GetWidth();
	int h = image.GetHeight();

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++) {
			int index = image.GetIndex(x, y);
			int channels = image.GetChannels();

			int thresholdIndex = 0;

			float threshold = 0;

			switch (thresholdMap)
			{
			case Threshold::ORDERED_8:
				thresholdIndex = (x % 8) + (y % 8) * 8;
				threshold = (float)Threshold8[thresholdIndex] / 64.0f;

				break;

			case Threshold::BLUENOISE_16:
				thresholdIndex = (x % 16) + (y % 16) * 16;
				threshold = (float)BlueNoise16[thresholdIndex] / 256.0f;

				break;
			}

			float octet = 1.0f / (float)factor;

			if (channels >= 3)
			{
				// convert 0-255 to 0-1
				float r = image.GetDataF(index + 0) / 255.0f;
				float g = image.GetDataF(index + 1) / 255.0f;
				float b = image.GetDataF(index + 2) / 255.0f;				

				//uint8_t convertedGray;
				float gray;

				switch (filter)
				{
				case DitherFilter::GRAYSCALE:
					gray = (r * 0.299f) + (g * 0.587f) + (b * 0.114f);
					gray = gray + octet * (threshold - 0.5f);
					gray = round(gray * (float)factor) / (float)factor;

					gray = ClampF(gray, 0.0f, 1.0f);
					gray *= 255.0f;

					//convertedGray = (uint8_t)round(gray * 255.0f);

					image.SetData(index + 0, gray);
					image.SetData(index + 1, gray);
					image.SetData(index + 2, gray);

					break;

				case DitherFilter::FULLCOLOR:
					r = r + octet * (threshold - 0.5f);
					r = round(r * (float)factor) / (float)factor;

					g = g + octet * (threshold - 0.5f);
					g = round(g * (float)factor) / (float)factor;

					b = b + octet * (threshold - 0.5f);
					b = round(b * (float)factor) / (float)factor;

					r = ClampF(r, 0.0f, 1.0f);
					g = ClampF(g, 0.0f, 1.0f);
					b = ClampF(b, 0.0f, 1.0f);

					image.SetData(index + 0, r * 255.0f);
					image.SetData(index + 1, g * 255.0f);
					image.SetData(index + 2, b * 255.0f);
					break;
				}
			}
			else
			{
				if (filter == DitherFilter::GRAYSCALE)
				{
					float total = 0;

					for (int i = 0; i < channels; i++)
					{
						total += image.GetDataF(index + i);
					}

					total /= (float)channels;
					total /= 255.0f;

					//total = (float)y / (float)(h - 1);

					total = total + octet * (threshold - 0.5f);
					total = round(total * (float)factor) / (float)factor;

					total = ClampF(total, 0.0f, 1.0f);

					for (int i = 0; i < channels; i++)
					{
						image.SetData(index + i, total * 255.0f);
					}
				}
				else if (filter == DitherFilter::FULLCOLOR)
				{
					for (int i = 0; i < channels; i++)
					{
						float c = image.GetDataF(index + i) / 255.0f;

						c = c * octet * (threshold - 0.5f);
						c = round(c * (float)factor) / (float)factor;

						c = ClampF(c, 0.0f, 1.0f);

						image.SetData(index + i, c * 255.0f);
					}
				}
			}
		}
	}
};