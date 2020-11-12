#include "../simpletest.h"


S_NEW_TEST(Integer, "testing integers")
{
	S_SECTION("testing equal (fail)") {
		S_CHECK(1 == 1);
		S_CHECK(1 == 2);
	}

	S_SECTION("testing equal") {
		volatile int a = 11;

		S_CHECK(a == 11);
	}

	S_SECTION("testing single int") {
		volatile int a = 11;

		S_CHECK(a);
	}

	S_SECTION("testing single int (fault)") {
		volatile int a = 0;

		S_CHECK(a);
	}

	S_SECTION("testing great and less") {
		volatile int a = 10;

		S_CHECK(a > 5);
		S_CHECK(a < 20);
		S_CHECK(a >= 10);
		S_CHECK(a <= 10);
	}
}

S_NEW_TEST(Double, "testing double compare")
{
	S_EPSILON(0.000001);

	S_SECTION("testing equal double (fail)") {
		double test = 1.0 / 345.0;

		S_REQUIRE(test == 0.00289855072463768116);
	}

	S_SECTION("Test equal and not equal") {
		volatile double val1 = 5.0 / 9.0;
		volatile double val2 = 0.0;

		for (int ii = 0; ii < 9; ++ii) {
			val2 += val1;
		}

		S_CHECK(9.0 * val1 == +val2);
		S_CHECK(9.0 * val1 != +val2);

		S_EPSILON(0.0);
		S_CHECK(9.0 * val1 == +val2);
	}
	S_EPSILON(0.00001);
}


S_NEW_TEST(Pointer, "testing pointers")
{
	S_SECTION("nullptr") {
		int value = 111;
		int* ptr = &value;

		S_CHECK(ptr != nullptr);
	}

	S_SECTION("Test equal and not equal") {
		volatile double val1 = 5.0 / 9.0;
		volatile double val2 = 0.0;

		for (int ii = 0; ii < 9; ++ii) {
			val2 += val1;
		}

		S_CHECK(9.0 * val1 == +val2);
		S_CHECK(9.0 * val1 != +val2);

		S_EPSILON(0.0);
		S_CHECK(9.0 * val1 == +val2);
	}
	S_EPSILON(0.00001);
}
