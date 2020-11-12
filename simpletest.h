#ifndef SIMPLETEST_H
#define SIMPLETEST_H

#pragma once

#include <csignal>
#include <stdexcept>
#include <string>
#include <limits>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "bashcolor.h"

#ifdef SIMPLETEST_USING_SIMPLEARGS
#include "simpleargs.h"
#endif

#define S_INTERNAL_CHECK(expr, isreq)  \
	try { \
		SimpleTest::Tester::instance().check(#expr, SimpleTest::Decomposer() <= expr, isreq); \
	} catch (const SimpleTest::RequireException& error) { \
		throw error; \
	} catch (const std::exception& error) { \
		SimpleTest::Tester::instance().printException(error); \
	}

#ifndef S_PRINT
	#define S_PRINT(str, ... )      printf(str, ##__VA_ARGS__ )
#endif

#define S_NEW_TEST(name, descr) void SimpleTest_##name(void)
#define S_SECTION(descr)        if(SimpleTest::Tester::instance().section(descr))
#define S_REQUIRE(expr)         S_INTERNAL_CHECK(expr, true)
#define S_CHECK(expr)           S_INTERNAL_CHECK(expr, false)
#define S_EPSILON(epsilon)      { SimpleTest::Tester::instance().setEpsilon(epsilon); }
#define S_TEXT_FAULT(x)         (rBashColor::getColor(rBashColor::RED, rBashColor::UNUSED, rBashColor::NONE, (x)).c_str())
#define S_TEXT_SUCCESS(x)       (rBashColor::getColor(rBashColor::GREEN, rBashColor::UNUSED, rBashColor::NONE, (x)).c_str())
#define S_TEXT_SECTION(x)       (rBashColor::getColor(rBashColor::BLUE, rBashColor::UNUSED, rBashColor::BOLD, (x)).c_str())
#define S_TEXT_INFO(x)          (rBashColor::getColor(rBashColor::GREEN, rBashColor::UNUSED, rBashColor::UNDERLINE, (x)).c_str())
#define S_TEXT_RESET(x)         (rBashColor::getReset((x)).c_str())

#define S_DBL_EQ(x, y)          (std::abs((x) - (y)) <= std::numeric_limits<double>::epsilon())
#define S_DBL_NEQ(x, y)         (std::abs((x) - (y)) >  std::numeric_limits<double>::epsilon())
#define S_FLT_EQ(x, y)          (std::abs((x) - (y)) <= std::numeric_limits<float>::epsilon())
#define S_FLT_NEQ(x, y)         (std::abs((x) - (y)) >  std::numeric_limits<float>::epsilon())


namespace SimpleTest
{

class Tester;

extern void handlerSimpleTest(int a);

class RequireException: public std::exception
{
private:
	std::string m_expression;
	std::string m_expantion;

public:
	RequireException(std::string expression, std::string expantion) :
		m_expression(expression),
		m_expantion (expantion ) {}

	virtual const char* getExpression() const noexcept { return m_expression.c_str(); }
	virtual const char* getExpantion()  const noexcept { return m_expantion.c_str(); }
};

class IExpr
{
protected:
	bool        m_result;
	std::string m_oper;

public:
	IExpr(bool result, const std::string& oper) : m_result(result), m_oper(oper) {}
	virtual bool getResult() const { return m_result; }
	virtual std::string getExpansion() const = 0;
};


std::stringstream& operator<< (std::stringstream& ss, const char* str)
{
	ss << std::string(str);
	return ss;
}

std::stringstream& operator<< (std::stringstream& ss, const void* ptr)
{
	std::intptr_t valptr = reinterpret_cast<std::intptr_t>(ptr);
	ss << "0x" << std::setfill('0') << std::setw(sizeof(int) / 4) << std::hex << valptr;
	return ss;
}

template<typename left_T, typename right_T>
class BoolResult : public IExpr
{
public:

	left_T  m_left;
	right_T m_right;

	virtual std::string getExpansion() const
	{
		std::stringstream ss;

		ss.precision(std::max(std::numeric_limits<left_T>::max_digits10, std::numeric_limits<right_T>::max_digits10));
		ss << m_left << " " << m_oper << " " << m_right;
		return ss.str();
	}

	BoolResult(bool result, const left_T& lval, const std::string& oper, const right_T& rval) :
		IExpr(result, oper), m_left(lval), m_right(rval) {}
};





class Tester
{
// Singleton
private:
	Tester(); //generating
	Tester(const Tester&) = delete;
	Tester& operator=(Tester&) = delete;
public:
	static Tester& instance() { static Tester Singleton; return Singleton; }

private:
	enum Flag
	{
		NONE = 0,
		AUTO,
		RUN,
	};

	const unsigned int FLAG_MANUALRUN = 0x00000001;

	typedef void(* TestFunc)(void);

	struct Item
	{
		Item(std::string name, std::string descr, TestFunc func) :
				m_descr(descr), m_name(name) { m_func = func; }
		std::string m_descr;
		std::string m_name;
		TestFunc    m_func;
		Flag        m_flags = Flag::AUTO;
	};

	virtual ~Tester();

public:
	int  check(const std::string& strexpr, const IExpr& expr, bool isRequire);
	int  section(std::string descr);
	void init(); // generating
	void args(int argc, const char** argv);
	int  run();
	void printException(const std::exception& error);
	void printResult(bool result, const std::string& expression, const std::string& expantion, bool isrequire);
	bool isColored() const { return m_colored; }
	double getEpsilon() const { return m_epsilon; }
	void   setEpsilon(double epsilon) { m_epsilon = epsilon; }

public:
	std::vector<Item*> m_tests;
	unsigned int       m_flags          = 0;
	unsigned int       m_curSection     = 0;
	unsigned int       m_curTest        = 1;
	unsigned int       m_faultSection   = 0;
	unsigned int       m_successSection = 0;
	unsigned int       m_faultTests     = 0;
	unsigned int       m_successTests   = 0;
	double             m_epsilon        = std::numeric_limits<double>::epsilon();

private:
	bool m_colored = true;
	bool m_moreOut = false;
};

template<typename left_T>
class BoolExpr : public IExpr
{
private:
	left_T m_expr;

public:
	BoolExpr(left_T& expr) :
		IExpr(expr, ""),
		m_expr(expr) {}

	BoolResult<left_T, double const&> const operator == (double const& rvalue ) {
		return BoolResult<left_T, double const&>( std::abs(m_expr - rvalue) <= Tester::instance().getEpsilon(), m_expr, "=d=", rvalue );
	}

	template<typename right_T>
	BoolResult<left_T, right_T const&> const operator == (right_T const& rval ) {
		return BoolResult<left_T, right_T const&>( m_expr == rval, m_expr, "==", rval );
	}

	BoolResult<left_T, double const&> const operator != (double const& rvalue ) {
		return BoolResult<left_T, double const&>( std::abs(m_expr - rvalue) > Tester::instance().getEpsilon(), m_expr, "!d=", rvalue );
	}

	template<typename right_T>
	BoolResult<left_T, right_T const&> const operator != (right_T const& rval ) {
		return BoolResult<left_T, right_T const&>( m_expr != rval, m_expr, "==", rval );
	}

	template<typename right_T>
	BoolResult<left_T, right_T const&> const operator > (right_T const& rval ) {
		return BoolResult<left_T, right_T const&>( m_expr > rval, m_expr, ">", rval );
	}

	template<typename right_T>
	BoolResult<left_T, right_T const&> const operator >= (right_T const& rval ) {
		return BoolResult<left_T, right_T const&>( m_expr >= rval, m_expr, ">=", rval );
	}

	template<typename right_T>
	BoolResult<left_T, right_T const&> const operator < (right_T const& rval ) {
		return BoolResult<left_T, right_T const&>( m_expr < rval, m_expr, "<", rval );
	}

	template<typename right_T>
	BoolResult<left_T, right_T const&> const operator <= (right_T const& rval ) {
		return BoolResult<left_T, right_T const&>( m_expr <= rval, m_expr, "<=", rval );
	}

/*
	auto operator != ( bool rhs ) -> BinaryExpr<LhsT, bool> const {
		return BinaryExpr<LhsT, bool>( m_lhs != rhs, m_lhs, "!=", rhs );
	}
*/
	virtual std::string getExpansion() const
	{
		std::stringstream ss;

		ss.precision(std::numeric_limits<left_T>::max_digits10);
		ss << m_expr;
		return ss.str();
	}
};


class Decomposer
{
public:
	template<typename T>
	BoolExpr<T const&> operator <= (T const& rvalue) { return BoolExpr<T const&>(rvalue); }

	BoolExpr<double> operator <= (double rvalue) { return BoolExpr<double>(rvalue); }
//	BoolExpr<bool> operator <= (bool value) { return BoolExpr<bool>(value); }
};

}

#endif // SIMPLETEST_H
