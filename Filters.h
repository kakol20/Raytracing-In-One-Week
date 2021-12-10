#ifndef FILTERS_H
#define FILTERS_H

#include "Image.h"

enum class DitherFilter
{
	GRAYSCALE, FULLCOLOR
};

enum class Threshold
{
	ORDERED_8, BLUENOISE_16
};

float ClampF(float val, float low, float high);

void Grayscale(Image & image);

void OrderedDithering(Image & image, DitherFilter filter, Threshold thresholdMap, int factor);

#endif // !FILTERS_H
