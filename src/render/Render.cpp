#include <iostream>

#include "Render.h"

Render::Render() {
	sAppName = "Raytracing with olc::PixelGameEngine";

	m_useThreads = 6;
	m_tileSize = 32;

	m_mode = Render::INIT;
}

bool Render::OnUserCreate() {
	m_threads.reserve(m_useThreads);

	m_render = olc::Sprite(ScreenWidth(), ScreenHeight());

	// ----- GENERATE TILES -----

	int maxXTiles = ScreenWidth() < m_tileSize ? 1 : ScreenWidth() / m_tileSize;
	int maxYTiles = ScreenHeight() < m_tileSize ? 1 : ScreenHeight() / m_tileSize;
	int maxTiles = maxXTiles * maxYTiles;

	//int xTileSize = Float::Round(Float(ScreenWidth()) / maxXTiles).ToInt();
	//int yTileSize = Float::Round(Float(ScreenHeight()) / maxYTiles).ToInt();

	int xTileSize = static_cast<int>(std::roundf(static_cast<float>(ScreenWidth())) / static_cast<float>(maxXTiles));
	int yTileSize = static_cast<int>(std::roundf(static_cast<float>(ScreenHeight())) / static_cast<float>(maxYTiles));

	int widthModulo = ScreenWidth() % maxXTiles;
	int heightModulo = ScreenHeight() % maxYTiles;

	int y = 0;
	int countY = 0;
	while (y < ScreenHeight()) {
		m_yTileCount++;

		int addY = heightModulo > 0 ? yTileSize + 1 : yTileSize;
		int maxY = y + addY;
		maxY = maxY > ScreenHeight() ? ScreenHeight() : maxY;

		int l_widthModulo = widthModulo;

		int x = 0;
		int xTileCount = 0;
		int countX = 0;
		while (x < ScreenWidth()) {
			xTileCount++;

			int addX = l_widthModulo > 0 ? xTileSize + 1 : xTileSize;
			int maxX = x + addX;
			maxX = maxX > ScreenWidth() ? ScreenWidth() : maxX;

			/*
			struct Tile {
				int minX, minY, maxX, maxY;
				bool tileComplete;
				int tileX, tileY;
				unsigned int seed;
			};
			*/
			m_tiles.push_back({ x, y, maxX, maxY, false, countX, countY, 0 });

			x = maxX;
			l_widthModulo--;
			countX++;
		}

		m_xTileCount = xTileCount;

		y = maxY;
		heightModulo--;
		countY++;
	}

	std::reverse(m_tiles.begin(), m_tiles.end());

	m_mode = Render::START;

	return true;
}

bool Render::OnUserUpdate(float fElapsedTime) {
	if (m_mode == Render::START) {
		if (!RunMode()) {
			m_mode = Render::END;
		}
	}
	else if (m_mode == Render::RENDERING) {
		// draw and wait for threads to finish
		bool allFinished = true;

		m_mutex.lock();

		// -- check for thread finish --
		for (auto it = m_threads.begin(); it != m_threads.end(); it++) {
			E_Thread& l_thread = (*it);

			if (!l_thread.finished) {
				allFinished = false;
				break;
			}
		}

		if (allFinished) {
			m_mode = Render::END;

			// -- join threads --
			for (auto it = m_threads.begin(); it != m_threads.end(); it++) {
				E_Thread& l_thread = (*it);

				l_thread.thread.join();
			}
		}
		else {
			Clear(olc::BLACK);
			DrawSprite(olc::vi2d(0, 0), &m_render);
		}

		m_mutex.unlock();
	}
	// else do nothing
	return true;
}

bool Render::RunMode() {
	// start multi-threading render
	m_tilesRendered = 0;
	m_nextAvailable = 0;
	size_t threadIndex = 0;

	m_mode = Render::RENDERING;

	for (size_t i = 0; i < m_useThreads; i++) {
		m_threads.push_back({ std::thread(&Render::RenderTile, this, m_nextAvailable, threadIndex), false });
		m_nextAvailable++;
		threadIndex++;
	}

	return true;
}

void Render::RenderTile(const size_t& startIndex, const size_t threadIndex) {
	if (startIndex < m_tiles.size()) {
		// set seed
		// start time
		//     Render();
		// end time
		// Assign tile color
		// Log time elapsed

		auto start = std::chrono::high_resolution_clock::now();
		RenderPixel(m_tiles[startIndex].minX, m_tiles[startIndex].minY, m_tiles[startIndex].maxX, m_tiles[startIndex].maxY);
		auto end = std::chrono::high_resolution_clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

		auto minDur = std::chrono::microseconds(2);
		if (dur > minDur) {
			std::this_thread::sleep_for(minDur);
		}

		// render next tile

		m_mutex.lock();

#ifdef _DEBUG
		std::cout << "Rendered tile #" << startIndex << '\n';
#endif // _DEBUG

		size_t nextIndex = m_nextAvailable;
		m_nextAvailable++;

		m_mutex.unlock();

		RenderTile(nextIndex, threadIndex);
	}
	else {
		m_mutex.lock();
		m_threads[threadIndex].finished = true;
		m_mutex.unlock();
	}
}

void Render::RenderPixel(const int& minX, const int& minY, const int& maxX, const int& maxY) {
	for (int x = minX; x < maxX; x++) {
		for (int y = minY; y < maxY; y++) {
			m_mutex.lock();

			m_render.SetPixel(olc::vi2d(x, y), olc::Pixel(x, y, 0, 255));

			m_mutex.unlock();
		}
	}
}