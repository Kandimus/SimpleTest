#include <iostream>
#include "../simpletest.h"

using namespace std;


int main(int argc, const char** argv)
{
	rSimpleTest::Instance().Args(argc, argv);
	rSimpleTest::Instance().Run();

	return 0;
}
