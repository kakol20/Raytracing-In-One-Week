#include <iostream>

#include "../maths/Maths.h"
#include "../maths/Vector3D.h"
#include "../utility/Random.h"

#include "Render.h"

Render::Render() {
	sAppName = "Raytracing with olc::PixelGameEngine";

	m_useThreads = 12;
	m_tileSize = 32;

	m_mode = Render::INIT;
}

bool Render::OnUserCreate() {
	m_threads.reserve(m_useThreads);

	m_render = olc::Sprite(ScreenWidth(), ScreenHeight());

	// ----- GENERETE SCENE -----

	m_background.LoadFromFile("images/UV_checker.jpg");

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

	//std::reverse(m_tiles.begin(), m_tiles.end());
	ShuffleTiles();

	m_mode = Render::START;

	return true;
}

bool Render::OnUserUpdate(float fElapsedTime) {
	if (m_mode == Render::START) {
		if (!RunMode()) {
			m_mode = Render::END;
		}
	} else if (m_mode == Render::RENDERING) {
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

			Clear(olc::BLACK);
			DrawSprite(olc::vi2d(0, 0), &m_render);
		} else {
			Clear(olc::BLACK);
			DrawSprite(olc::vi2d(0, 0), &m_render);
		}

		m_mutex.unlock();
	} else {
		/*Clear(olc::BLACK);
		DrawSprite(olc::vi2d(0, 0), &m_render);*/
	}
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

		std::chrono::duration<double, std::micro> dur = end - start;

		/*int minDur = 8333;
		if (dur.count() < static_cast<double>(minDur)) {
			std::this_thread::sleep_for(std::chrono::microseconds(minDur));
		}*/

		// render next tile

		m_mutex.lock();

#ifdef _DEBUG
		std::cout << "Rendered tile #" << startIndex << " for " << dur << '\n';
#endif // _DEBUG

		size_t nextIndex = m_nextAvailable;
		m_nextAvailable++;

		m_mutex.unlock();

		RenderTile(nextIndex, threadIndex);
	} else {
		m_mutex.lock();
		m_threads[threadIndex].finished = true;
		m_mutex.unlock();
	}
}

void Render::RenderPixel(const int& minX, const int& minY, const int& maxX, const int& maxY) {
	for (int x = minX; x < maxX; x++) {
		for (int y = minY; y < maxY; y++) {
			m_mutex.lock();

			int invY = ScreenHeight() - y - 1;

			E_Pixel col;

			//col.LinearToSRGB();

			// ----- RENDER START -----

			// temp render to test classes

			//const Vector3D point(static_cast<float>(x), static_cast<float>(y));

			Vector3D point, pov;

			point += Vector3D::Right * static_cast<float>(x);
			point += Vector3D::Up * static_cast<float>(invY);

			float dist = -std::fmaxf(static_cast<float>(ScreenWidth()), static_cast<float>(ScreenHeight()));
			dist /= 3.f;
			//Vector3D pov(0.f, 0.f, );

			pov -= Vector3D::Forward * dist;

			Vector3D delta = point - pov;
			delta.Normalize();
			delta = Vector3D::UVSphere(delta);
			//delta += Vector3D::One;
			//delta /= 2.f;
			//delta *= 255.f;

			delta *= Vector3D(static_cast<float>(m_background.width), static_cast<float>(m_background.height));

			olc::vi2d pos(static_cast<int32_t>(delta.GetX()), static_cast<int32_t>(delta.GetY()));

			col = m_background.GetPixel(pos);

			// ----- RENDER END -----

			col.Dither(x, y, 31);

			m_render.SetPixel(olc::vi2d(x, y), col.GetOLC());

			m_mutex.unlock();
		}
	}
}

void Render::ShuffleTiles() {
	size_t i = m_tiles.size() - 1;
	while (i >= 0) {
		//size_t swap = (size_t)round(Random::RandomFloat(0.f, (float)i));
		size_t swap = (size_t)Random::RandomUInt() % (i + 1);

		if (swap < i) {
			std::swap(m_tiles[i], m_tiles[swap]);
		}

		if (i == 0) break;

		i--;
	}
}