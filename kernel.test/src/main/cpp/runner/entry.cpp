#include "runner.hpp"

int main(int /*argc*/, char** /*argv*/)
{
	Runner::runTests();
	return Runner::printTestResults();
}
