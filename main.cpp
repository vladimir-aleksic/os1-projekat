#include "kernel.h";

extern int userMain(int, char**);

int main(int argc, char** argv) {
	Kernel::init();

	int result = userMain(argc, argv);

	Kernel::restore();

	return result;
}
