#include <iostream>
#include "simpletest.h"

int main(int argc, const char** argv)
{
	SimpleTest::Tester::instance().args(argc, argv);
	SimpleTest::Tester::instance().run();

	return 0;
}
