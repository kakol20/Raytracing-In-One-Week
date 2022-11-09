#pragma once

#include <mutex>
#include <thread>
#include <vector>

#include "../../ext/olcPixelGameEngine.h"

#include "../extended/E_Pixel.h"

class Render : public olc::PixelGameEngine {
public:
	enum Mode {
		START, RENDERING, END, INIT
	};

	// ----- PIXEL GAME ENGINE -----

	Render();

	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;

private:
	// ----- RAY TRACING -----

	bool RunMode();
	void RenderTile(const size_t& startIndex, const size_t threadIndex);
	void RenderPixel(const int& minX, const int& minY, const int& maxX, const int& maxY);

	olc::Sprite m_render;

	// ----- MULTI-THREADING -----

	struct E_Thread {
		std::thread thread;
		bool finished;
	};

	size_t m_useThreads;
	std::mutex m_mutex;
	Mode m_mode;

	std::vector<E_Thread> m_threads;

	// ----- TILES -----

	struct Tile {
		int minX, minY, maxX, maxY;
		bool tileComplete;
		int tileX, tileY;
		unsigned int seed;
	};

	int m_tileSize;
	int m_tilesRendered;
	int m_nextAvailable;
	int m_xTileCount, m_yTileCount;
	std::vector<Tile> m_tiles;

	void ShuffleTiles();

	// ----- WINDOW -----
};