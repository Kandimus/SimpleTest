#ifndef SIMPLETEST_H
#define SIMPLETEST_H

#pragma once

#include <csignal>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef SIMPLETEST_USING_SIMPLEARGS
#include "simpleargs.h"
#endif

#define S_INTERNAL_CHECK(expr, isreq)  \
	try { \
		if (!rSimpleTest::instance().check(#expr, expr, isreq)) { throw RequireException(#expr); } \
	} catch (const RequireException& error) { \
		throw error; \
	} catch (const std::exception& error) { \
		rSimpleTest::instance().printException(error); \
	}

#ifndef S_PRINT
	#define S_PRINT(str, ... )      printf(str, ##__VA_ARGS__ )
#endif

#define S_NEW_TEST(name, descr) void SimpleTest_##name(void)
#define S_SECTION(descr)        if(rSimpleTest::instance().section(descr))
#define S_REQUIRE(expr)         S_INTERNAL_CHECK(expr, true)
#define S_CHECK(expr)           S_INTERNAL_CHECK(expr, false)
#define S_TEXT_FAULT(x)         (rBashColor::getColor(rBashColor::RED, rBashColor::UNUSED, rBashColor::NONE, (x)).c_str())
#define S_TEXT_SUCCESS(x)       (rBashColor::getColor(rBashColor::GREEN, rBashColor::UNUSED, rBashColor::NONE, (x)).c_str())
#define S_TEXT_SECTION(x)       (rBashColor::getColor(rBashColor::BLUE, rBashColor::UNUSED, rBashColor::BOLD, (x)).c_str())
#define S_TEXT_INFO(x)          (rBashColor::getColor(rBashColor::GREEN, rBashColor::UNUSED, rBashColor::UNDERLINE, (x)).c_str())
#define S_TEXT_RESET(x)         (rBashColor::getReset((x)).c_str())

#define S_DBL_EQ(x, y)          (std::abs((x) - (y)) <= std::numeric_limits<double>::epsilon())
#define S_DBL_NEQ(x, y)         (std::abs((x) - (y)) >  std::numeric_limits<double>::epsilon())
#define S_FLT_EQ(x, y)          (std::abs((x) - (y)) <= std::numeric_limits<float>::epsilon())
#define S_FLT_NEQ(x, y)         (std::abs((x) - (y)) >  std::numeric_limits<float>::epsilon())


extern void handlerSimpleTest(int a);

class RequireException: public std::exception
{
private:
	std::string m_error;

public:
	RequireException(std::string error) : m_error(error) {}
	virtual const char* what() const noexcept { return m_error.c_str(); }
};


class rBashColor
{
public:
	enum Color
	{
		UNUSED = -1,
		BLACK = 0,
		RED,
		GREEN,
		YELLOW,
		BLUE,
		MAGENTA,
		CYAN,
		WHITE,
	};

	enum Style
	{
		NONE      = 0x00,
		NORMAL    = 0x01,
		BOLD      = 0x02,
		UNDERLINE = 0x04,
		BLINK     = 0x08,
		INVERT    = 0x10,
		INVISIBLE = 0x20,
	};

	#define COLOR_RESET "\x1b[0m"

	static std::string getReset(bool iscolored)
	{
		return iscolored ? COLOR_RESET : "";
	}

	#define APPLY_STYLE(x, y) if (style & (x)) { ascii += (ascii.size() ? std::string(";") : std::string("")) + #y; }

	static std::string getColor(Color txt, Color bg, Style style, bool iscolored)
	{
		static const std::string escape     = "\x1b[";
		static const std::string colors[8]  = {"30", "31", "32", "33", "34", "35", "36", "37"};
		static const std::string bground[8] = {"40", "41", "42", "43", "44", "45", "46", "47"};

		std::string ascii = "";

		if (!iscolored) {
			return ascii;
		}

		APPLY_STYLE(Style::NORMAL   , 10);
		APPLY_STYLE(Style::BOLD     ,  1);
		APPLY_STYLE(Style::UNDERLINE,  4);
		APPLY_STYLE(Style::BLINK    ,  5);
		APPLY_STYLE(Style::INVERT   ,  7);
		APPLY_STYLE(Style::INVISIBLE,  8);

		if (txt != Color::UNUSED) {
			ascii += (ascii.size() ? std::string(";") : std::string("")) + colors[txt];
		}

		if (bg != Color::UNUSED) {
			ascii += (ascii.size() ? std::string(";") : std::string("")) + bground[txt];
		}

		return ascii.size() ? escape + ascii + "m" : ascii;
	}

	std::string setColor(Color txt, Color bg, Style style, const std::string str, bool iscolored, bool isescaped)
	{
		std::string result = str;
		std::string escape = getColor(txt, bg, style, iscolored);

		if (escape.size()) {
			result = escape + result;

			if (isescaped) {
				result += COLOR_RESET;
			}
		}

		return result;
	}
};


class rSimpleTest
{
// Singleton
private:
	rSimpleTest(); //generating
	rSimpleTest(const rSimpleTest&);
	rSimpleTest& operator=(rSimpleTest&);
public:
	static rSimpleTest &instance() { static rSimpleTest Singleton; return Singleton; }

private:
	enum rItemFlag
	{
		NONE = 0,
		AUTO,
		RUN,
	};

	const unsigned int FLAG_MANUALRUN = 0x00000001;

	typedef void(* rTestFunc)(void);

	struct rItemType
	{
		rItemType(std::string name, std::string descr, rTestFunc func) :
				m_descr(descr), m_name(name) { m_func = func; }
		std::string m_descr;
		std::string m_name;
		rTestFunc   m_func;
		rItemFlag   m_flags = rItemFlag::AUTO;
	};

	virtual ~rSimpleTest();

public:
	int  check(const std::string& strexpr, bool expr, bool isRequire);
	int  section(std::string descr);
	void init(); // generating
	void args(int argc, const char** argv);
	int  run();
	void printException(const std::exception& error);
	void printCheck(const std::string& expr);
	void printRequire(const std::string& expr);
	void printSuccess(const std::string& expr, bool isreq);
	bool isColored();

public:
	std::vector<rItemType* > m_tests;
	unsigned int             m_flags          = 0;
	unsigned int             m_curSection     = 0;
	unsigned int             m_curTest        = 1;
	unsigned int             m_faultSection   = 0;
	unsigned int             m_successSection = 0;
	unsigned int             m_faultTests     = 0;
	unsigned int             m_successTests   = 0;

private:
	bool m_colored = true;
	bool m_moreOut = false;
};

#endif // SIMPLETEST_H
