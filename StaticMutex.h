#pragma once
#include <mutex>

class StaticMutex {
public:
	StaticMutex() {};
	~StaticMutex() {};

	static std::mutex s_mtx;
};