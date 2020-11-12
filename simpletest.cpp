
#include "simpletest.h"

namespace SimpleTest
{

void handlerSimpleTest(int a)
{
	printf("Section %i: %sSignal %i!%s Exit\n",
		   SimpleTest::Tester::instance().m_curSection,
		   S_TEXT_FAULT(SimpleTest::Tester::instance().isColored()),
		   a,
		   S_TEXT_RESET(SimpleTest::Tester::instance().isColored()));
	exit(1);
}

Tester::~Tester()
{
	for (auto& t : m_tests) {
		delete t;
	}
	m_tests.clear();
}


int Tester::check(const std::string& strexpr, const IExpr& expr, bool isRequire)
{
	printResult(expr.getResult(), strexpr, expr.getExpansion(), isRequire);

	if (expr.getResult()) {
		++m_successSection;
		++m_successTests;
		return true;
	}

	++m_faultSection;
	++m_faultTests;

	if(isRequire) {
		throw RequireException(strexpr, expr.getExpansion());
	}

	return true;
}

int Tester::section(std::string descr)
{
	++m_curSection;
	S_PRINT("Section %i: %s\n", m_curSection, descr.c_str());
	return true;
}

void Tester::args(int argc, const char** argv)
{
#ifdef SIMPLETEST_USING_SIMPLEARGS
	rSimpleArgs::instance()
			.addSwitch("nocolor", 'n')
			.addSwitch("all", 'a')
			.addOption("test", 't');

	rSimpleArgs::instance().parse(argc, argv);
	m_colored = rSimpleArgs::instance().isSet("nocolor");
	m_moreOut = rSimpleArgs::instance().isSet("all");

	for (unsigned int ii = 0; ii < rSimpleArgs::instance().getCountArgument(); ++ii) {
		m_flags |= FLAG_MANUALRUN;
		for (auto t : m_tests) {
			if(t->m_name == rSimpleArgs::instance().getArgument()) {
				t->m_flags = rItemFlag::RUN;
				break;
			}
		}
	}
#else
	for (int ii = 1; ii < argc; ++ii) {
		std::string str = argv[ii];

		if (str == "--nocolor") {
			m_colored = false;
		} else if (str == "--all") {
			m_moreOut = true;
		} else {
			m_flags |= FLAG_MANUALRUN;
			for (auto t : m_tests) {
				if(t->m_name == argv[ii]) {
					t->m_flags = Flag::RUN;
					break;
				}
			}
		}
	}
#endif
}

int Tester::run()
{
	signal(SIGFPE , handlerSimpleTest);
	signal(SIGSEGV, handlerSimpleTest);

	S_PRINT("%sSimpleTest v 0.1%s\n", S_TEXT_INFO(m_colored), S_TEXT_RESET(m_colored));
	for(auto t : m_tests) {
		if (((m_flags & FLAG_MANUALRUN) && (t->m_flags & Flag::RUN)) || !(m_flags & FLAG_MANUALRUN)) {
			S_PRINT("%s===== Test %u/%i [%s] %s%s\n",
					  S_TEXT_SECTION(m_colored), m_curTest, m_tests.size(), t->m_name.c_str(), t->m_descr.c_str(), S_TEXT_RESET(m_colored));

			m_curSection     = 0;
			m_faultSection   = 0;
			m_successSection = 0;

			try {
				t->m_func();
				S_PRINT("Test %i: %s%i%s/%s%i%s\n",
						  m_curTest,
						  S_TEXT_SUCCESS(m_colored), m_successSection, S_TEXT_RESET(m_colored),
						  S_TEXT_FAULT(m_colored), m_faultSection, S_TEXT_RESET(m_colored));

			} catch(const RequireException& error) {
				;
			} catch(const std::exception& error) {
				printException(error);
			}
			++m_curTest;
		}
	}

	S_PRINT("%s=== All tests%s: %s%i%s/%s%i%s\n",
			  S_TEXT_SECTION(m_colored), S_TEXT_RESET(m_colored),
			  S_TEXT_SUCCESS(m_colored), m_successTests, S_TEXT_RESET(m_colored),
			  S_TEXT_FAULT(m_colored), m_faultTests, S_TEXT_RESET(m_colored));

	return true;
}

void Tester::printException(const std::exception& error)
{
	S_PRINT("Section %i: %sException (%s)%s\n", m_curSection, S_TEXT_FAULT(m_colored), error.what(), S_TEXT_RESET(m_colored));
}

void Tester::printResult(bool result, const std::string& expression, const std::string& expantion, bool isrequire)
{
	std::string name = isrequire ? "Require" : "  Check";

	if (result) {
		if (!m_moreOut) {
			return;
		}

		S_PRINT("\t%s %sSUCCESS%s: %s\n",
				name.c_str(),
				S_TEXT_SUCCESS(m_colored), S_TEXT_RESET(m_colored), expression.c_str());
	} else {
		S_PRINT("\t%s   %sFAULT%s: %s%s%s ( %s%s%s )\n",
				name.c_str(),
				S_TEXT_FAULT(m_colored), S_TEXT_RESET(m_colored),
				S_TEXT_FAULT(m_colored), expression.c_str(), S_TEXT_RESET(m_colored),
				S_TEXT_FAULT(m_colored), expantion.c_str() , S_TEXT_RESET(m_colored));
	}
}

}
