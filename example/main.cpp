#include <iostream>
#include "../simpletest.h"

using namespace std;


int main(int argc, const char** argv)
{
	rSimpleTest::instance().args(argc, argv);
	rSimpleTest::instance().run();

	return 0;
}
