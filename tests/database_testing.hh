/*
 * Copyright (C) 2011-2016 Dmitry Marakasov
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

#include <algorithm>

#include "testing.hh"

#define CHECK_EXACT_MATCH(database, sample) \
	EXPECT_TRUE(database.CheckExactMatch(sample) == 1);

#define CHECK_NO_EXACT_MATCH(database, sample) \
	EXPECT_TRUE(database.CheckExactMatch(sample) == 0);

#define CHECK_CANONICAL_FORM(database, sample, expected) { \
		std::vector<std::string> suggestions; \
		EXPECT_TRUE(database.CheckCanonicalForm(sample, suggestions) == 1); \
		if (!suggestions.empty()) EXPECT_STRING(suggestions.front(), expected); \
	}

#define CHECK_CANONICAL_FORM2(database, sample, expected1, expected2) { \
		std::vector<std::string> suggestions; \
		EXPECT_TRUE(database.CheckCanonicalForm(sample, suggestions) == 2); \
		if (!suggestions.empty()) EXPECT_TRUE( \
				(suggestions.front() == expected1 && suggestions.back() == expected2) || \
				(suggestions.front() == expected2 && suggestions.back() == expected1)); \
	}

#define CHECK_CANONICAL_FORM_HAS(database, sample, expected) { \
		std::vector<std::string> suggestions; \
		EXPECT_TRUE(database.CheckCanonicalForm(sample, suggestions) >= 1); \
		if (!suggestions.empty()) EXPECT_TRUE(std::find(suggestions.begin(), suggestions.end(), expected) != suggestions.end()); \
	}

#define CHECK_SPELLING(database, sample, expected, depth) { \
		std::vector<std::string> suggestions; \
		EXPECT_TRUE(database.CheckSpelling(sample, suggestions, depth) == 1); \
		if (!suggestions.empty()) EXPECT_STRING(suggestions.front(), expected); \
	}

#define CHECK_NO_SPELLING(database, sample, depth) { \
		std::vector<std::string> suggestions; \
		EXPECT_TRUE(database.CheckSpelling(sample, suggestions, depth) == 0); \
	}

#define CHECK_STRIPPED_STATUS(database, sample) { \
		std::vector<std::string> suggestions; \
		EXPECT_TRUE(database.CheckStrippedStatus(sample, suggestions) == 1); \
	}

#define CHECK_NO_STRIPPED_STATUS(database, sample) { \
		std::vector<std::string> suggestions; \
		EXPECT_TRUE(database.CheckStrippedStatus(sample, suggestions) == 0); \
	}
