/*
 * Copyright (C) 2011 Dmitry Marakasov
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

#include <streetmangler/database.hh>
#include <streetmangler/locale.hh>
#include "testing.hh"

#define CHECK_CANONICAL_FORM(sample, expected) { \
		std::vector<std::string> suggestions; \
		EXPECT_TRUE(db.CheckCanonicalForm(sample, suggestions) == 1); \
		if (!suggestions.empty()) EXPECT_STRING(suggestions.front(), expected); \
	}

#define CHECK_SPELLING(sample, expected) { \
		std::vector<std::string> suggestions; \
		EXPECT_TRUE(db.CheckSpelling(sample, suggestions) == 1); \
		if (!suggestions.empty()) EXPECT_STRING(suggestions.front(), expected); \
	}

#define CHECK_STRIPPED_STATUS(sample) { \
		std::vector<std::string> suggestions; \
		EXPECT_TRUE(db.CheckStrippedStatus(sample, suggestions) == 1); \
	}

#define CHECK_NO_STRIPPED_STATUS(sample) { \
		std::vector<std::string> suggestions; \
		EXPECT_TRUE(db.CheckStrippedStatus(sample, suggestions) == 0); \
	}

BEGIN_TEST()
	using StreetMangler::Database;
	using StreetMangler::Locale;

	/* assumes working locale, see locale_test */
	Locale locale("ru");

	Database db(locale);

	db.Add("улица Ленина");
	db.Add("Зелёная улица");
	db.Add("МКАД");

	/* simple matches */
	EXPECT_TRUE(db.CheckExactMatch("улица Ленина") == 1);
	EXPECT_TRUE(db.CheckExactMatch("Зелёная улица") == 1);
	EXPECT_TRUE(db.CheckExactMatch("улица Сталина") == 0);
	EXPECT_TRUE(db.CheckExactMatch("переулок Ленина") == 0);

	/* canonical forms (see also tokenizer_test.cc) */
	CHECK_CANONICAL_FORM("ул.Ленина", "улица Ленина");
	CHECK_CANONICAL_FORM("ул. Ленина", "улица Ленина");
	CHECK_CANONICAL_FORM("Ленина улица", "улица Ленина");
	CHECK_CANONICAL_FORM("Ленина ул", "улица Ленина");
	CHECK_CANONICAL_FORM("Ленина ул.", "улица Ленина");
	CHECK_CANONICAL_FORM("Ленина, ул", "улица Ленина");
	CHECK_CANONICAL_FORM("Ленина, ул.", "улица Ленина");
	CHECK_CANONICAL_FORM("Ленина,ул", "улица Ленина");
	CHECK_CANONICAL_FORM("Ленина,ул.", "улица Ленина");
	CHECK_CANONICAL_FORM("Ленина,улица", "улица Ленина");
	CHECK_CANONICAL_FORM("Ленина, улица", "улица Ленина");
	CHECK_CANONICAL_FORM("лЕНИНА, УЛИЦА", "улица Ленина");
	CHECK_CANONICAL_FORM("УЛИЦА ЛЕНИНА", "улица Ленина");
	CHECK_CANONICAL_FORM("   улица  ленина    ", "улица Ленина");
	CHECK_CANONICAL_FORM("\tулица\tленина\t", "улица Ленина");

	/* spelling */
	CHECK_SPELLING("улица Ленена", "улица Ленина");  /* letter changed */
	CHECK_SPELLING("улица Ленна", "улица Ленина");   /* letter removed */
	CHECK_SPELLING("улица Ленинаа", "улица Ленина"); /* letter added */
	CHECK_SPELLING("улица Леинна", "улица Ленина");  /* letters changed places */

	CHECK_SPELLING("улиа Ленина", "улица Ленина");   /* error in status part */
	CHECK_SPELLING("уилца Ленина", "улица Ленина");  /* error in status part */

	/* missing status part */
	CHECK_STRIPPED_STATUS("Ленина");
	CHECK_STRIPPED_STATUS("Зелёная");

	/* names with no status part originally should not be counted
	 * as stripped of status part */
	CHECK_NO_STRIPPED_STATUS("МКАД");

END_TEST()
