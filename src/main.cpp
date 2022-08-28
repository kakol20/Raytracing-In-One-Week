#include <iostream>

#ifdef _DEBUG
int main() {
#else
int WinMain() {
#endif // _DEBUG

	std::cout << "Hello World\n";
	std::cin.ignore();

	return  0;
}