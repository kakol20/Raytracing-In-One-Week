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
	output += "Hello World!\n";
	output += String::ToString(8022022);
	output += '\n';
	output += "Random unsigned int = ";
	output += String::ToString(Random::RandomUInt());
	output += '\n';
	output += "Random int = ";
	output += String::ToString(Random::RandInt());
	output += '\n';
	output += "Max Int = ";
	output += String::ToString(Random::MaxUInt);
	output += '\n';
	output += "Random float = ";
	output += String::ToString(Random::RandFloat());
	output += '\n';
	output += "Random float between -1 and 1 = ";
	output += String::ToString(Random::RandFloatRange(-1, 1));
	output += '\n';
	output += "Seed After = ";
	output += String::ToString(Random::Seed);
	output += '\n';

	FastWrite::Write(output);

	m_hdri.Write("temp/test.png", Image::ColorMode::sRGB);

    return true;
}
