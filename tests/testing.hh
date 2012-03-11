/*
 * Copyright (C) 2011-2012 Dmitry Marakasov
 *
 * This file is part of streetmangler.
 *
 * streetmangler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * streetmangler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with streetmangler.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <exception>

#define TEST_COLOR

#ifdef TEST_COLOR
#	define PASSED "\e[0;32mPASSED:\e[0m "
#	define FAILED "\e[1;31mFAILED:\e[0m "
#else
#	define PASSED "PASSED: "
#	define FAILED "FAILED: "
#endif

#define BEGIN_TEST() int main() { int ret = 0;

#define END_TEST() if (ret > 0) std::cerr << ret << " failures" << std::endl; return ret; }

#define EXPECT_STRING(expr, expected) { \
		std::string result = (expr); \
		if (result != expected) { \
			std::cerr << FAILED #expr " returned \"" << result << "\", while expected \"" << expected << "\"" << std::endl; ++ret; \
		} else { \
			std::cerr << PASSED #expr " == \"" << expected << "\"" << std::endl; \
		} \
	}

#define EXPECT_TRUE(expr) { \
		if (!(expr)) { \
			std::cerr << FAILED #expr << std::endl; ++ret; \
		} else { \
			std::cerr << PASSED #expr << std::endl; \
		} \
	}

#define EXPECT_EXCEPTION(expr, exception) { \
		bool correct_catch = false; \
		try { \
			expr; \
		} catch (exception &e) { \
			correct_catch = true; \
		} catch (...) { \
		} \
		if (correct_catch) { \
			std::cerr << PASSED "correctly cought " #exception << std::endl; \
		} else { \
			std::cerr << FAILED "didn't catch expected " #exception << std::endl; ++ret; \
		} \
	}

#define EXPECT_NO_EXCEPTION(expr) { \
		bool had_exception = false; \
		const char* what = NULL; \
		try { \
			expr; \
		} catch (std::exception& e) { \
			had_exception = true; \
			what = e.what(); \
		} catch (...) { \
			had_exception = true; \
		} \
		if (had_exception && what) { \
			std::cerr << FAILED "unexpected exception thrown: " << what << std::endl; ++ret; \
		} else if (had_exception) { \
			std::cerr << FAILED "unexpected unknown exception thrown" << std::endl; ++ret; \
		} else { \
			std::cerr << PASSED "no unexpected exceptinos thrown" << std::endl; \
		} \
	}
