#include "oof/oof.h"

#include "FastWrite.h"
#include "Random.h"
#include "String.h"

#include "Raytracing.h"

bool Raytracing::Init() {
	Random::Seed = 2790598843;

	m_hdri.Read("images/hdri/spruit_sunrise_2k.png", Image::ColorMode::sRGB);

	return true;
}

bool Raytracing::Run() {
	String output("");
	output += '\a';

	FastWrite::Write(output);

	m_hdri.Write("temp/test.png", Image::ColorMode::sRGB);

	return true;
}
