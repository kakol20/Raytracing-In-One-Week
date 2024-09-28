#include <iostream>

#include "managers/MainManager.h"

int main(int argc, char* argv[]) {
	/*std::cout << "Hello World\n";

	std::cout << "Press enter to exit...\n";
	std::cin.ignore();*/

	const int initInt = MainManager::Init();
	if (initInt != EXIT_SUCCESS) {
		MainManager::Shutdown();
		return initInt;
	}
	while (true) {
		const MainManager::UpdateType updateType = MainManager::Update();
		if (updateType == MainManager::UpdateType::BREAK) break;
		if (updateType == MainManager::UpdateType::CONTINUE) continue;
		MainManager::Render();
	}
	MainManager::Shutdown();

	return EXIT_SUCCESS;
}