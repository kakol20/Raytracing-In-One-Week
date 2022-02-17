#pragma once
#include <mutex>
#include "String.h"

class StaticMutex {
public:
	StaticMutex() {};
	~StaticMutex() {};

	static std::mutex s_mtx;

	static thread_local String s_output;
};