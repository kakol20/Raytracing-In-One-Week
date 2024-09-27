#include <iostream>

#include "managers/MainManager.h"

int main(int argc, char* argv[]) {
	/*std::cout << "Hello World\n";

	std::cout << "Press enter to exit...\n";
	std::cin.ignore();*/

	MainManager::Init();
	while (true) {
		if (!MainManager::Update()) break;
		MainManager::Render();
	}
	MainManager::Shutdown();

	return 0;
}