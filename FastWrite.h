#pragma once
#include "oof/oof.h"
#include "String.h"
class FastWrite {
public:
	FastWrite() {};
	~FastWrite() {};

	static void Write(std::string& output);
	static void Write(String& output);
};

