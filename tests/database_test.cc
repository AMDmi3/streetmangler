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
#include "database_testing.hh"

BEGIN_TEST()
	using StreetMangler::Database;
	using StreetMangler::Locale;

	/* assumes working locale, see locale_test */
	Locale locale("ru_RU");

	Database db(locale);

	db.Add("улица Ленина");
	db.Add("Зелёная улица");
	db.Add("МКАД");
	db.Add("улица Льва Толстого");

	/* simple matches */
	CHECK_EXACT_MATCH(db, "улица Ленина");
	CHECK_EXACT_MATCH(db, "Зелёная улица");
	CHECK_NO_EXACT_MATCH(db, "улица Сталина");
	CHECK_NO_EXACT_MATCH(db, "переулок Ленина");

	/* canonical forms (see also tokenizer_test.cc) */
	CHECK_CANONICAL_FORM(db, "ул.Ленина", "улица Ленина");
	CHECK_CANONICAL_FORM(db, "ул. Ленина", "улица Ленина");
	CHECK_CANONICAL_FORM(db, "Ленина улица", "улица Ленина");
	CHECK_CANONICAL_FORM(db, "Ленина ул", "улица Ленина");
	CHECK_CANONICAL_FORM(db, "Ленина ул.", "улица Ленина");
	CHECK_CANONICAL_FORM(db, "Ленина, ул", "улица Ленина");
	CHECK_CANONICAL_FORM(db, "Ленина, ул.", "улица Ленина");
	CHECK_CANONICAL_FORM(db, "Ленина,ул", "улица Ленина");
	CHECK_CANONICAL_FORM(db, "Ленина,ул.", "улица Ленина");
	CHECK_CANONICAL_FORM(db, "Ленина,улица", "улица Ленина");
	CHECK_CANONICAL_FORM(db, "Ленина, улица", "улица Ленина");
	CHECK_CANONICAL_FORM(db, "лЕНИНА, УЛИЦА", "улица Ленина");
	CHECK_CANONICAL_FORM(db, "УЛИЦА ЛЕНИНА", "улица Ленина");
	CHECK_CANONICAL_FORM(db, "   улица  ленина    ", "улица Ленина");
	CHECK_CANONICAL_FORM(db, "\tулица\tленина\t", "улица Ленина");

	CHECK_CANONICAL_FORM(db, "Толстого Льва улица", "улица Льва Толстого");

	/* spelling */
	CHECK_SPELLING(db, "улица Ленена", "улица Ленина");  /* letter changed */
	CHECK_SPELLING(db, "улица Ленна", "улица Ленина");   /* letter removed */
	CHECK_SPELLING(db, "улица Ленинаа", "улица Ленина"); /* letter added */
	CHECK_SPELLING(db, "улица Леинна", "улица Ленина");  /* letters changed places */

	CHECK_SPELLING(db, "улиа Ленина", "улица Ленина");   /* error in status part */
	CHECK_SPELLING(db, "уилца Ленина", "улица Ленина");  /* error in status part */

	CHECK_NO_SPELLING(db, "улица Феника");   /* >1 errors */
	CHECK_NO_SPELLING(db, "улица Ленинааа"); /* >1 errors */
	CHECK_NO_SPELLING(db, "ууулица Ленина"); /* >1 errors */
	CHECK_NO_SPELLING(db, "улица Линена");   /* >1 errors */

	/* missing status part */
	CHECK_STRIPPED_STATUS(db, "Ленина");
	CHECK_STRIPPED_STATUS(db, "Зелёная");

	/* names with no status part originally should not be counted
	 * as stripped of status part */
	CHECK_NO_STRIPPED_STATUS(db, "МКАД");

END_TEST()
