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
	db.Add("улица Лебедева-Кумача");
	db.Add("улица Верхний переулок");
	db.Add("Учительская улица");
	db.Add("улица Петра Безымянного");
	db.Add("улица Петро Безымянного");
	db.Add("1-я улица Строителей");
	db.Add("улица 3-го Интернационала");
	db.Add("Измайловский район");
	db.Add("район Измайловка");

	/*
	 * simple matches
	 */
	CHECK_EXACT_MATCH(db, "улица Ленина");
	CHECK_EXACT_MATCH(db, "Зелёная улица");
	CHECK_NO_EXACT_MATCH(db, "улица Сталина");
	CHECK_NO_EXACT_MATCH(db, "переулок Ленина");

	/*
	 * canonical forms (see also tokenizer_test.cc)
	 */
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

	/*
	 * spelling
	 */
	CHECK_SPELLING(db, "улица Ленена", "улица Ленина", 1);  /* letter changed */
	CHECK_SPELLING(db, "улица Ленна", "улица Ленина", 1);   /* letter removed */
	CHECK_SPELLING(db, "улица Ленинаа", "улица Ленина", 1); /* letter added */
	CHECK_SPELLING(db, "улица Леинна", "улица Ленина", 1);  /* letters swapped (should count as 1 error) */

	CHECK_SPELLING(db, "улиа Ленина", "улица Ленина", 1);   /* error in status part */
	CHECK_SPELLING(db, "уилца Ленина", "улица Ленина", 1);  /* letters swapped in status part (should count as 1 error) */

	CHECK_SPELLING(db, "Учительская улицца", "Учительская улица", 1); /* error in status part, reorder issue */

	/* error prioriyy */
	CHECK_SPELLING(db, "улица Безымянного Петра", "улица Петра Безымянного", 0);
	CHECK_SPELLING(db, "улица Безымянного Петра", "улица Петра Безымянного", 1);

	/* > 1 errors */
	CHECK_NO_SPELLING(db, "улица Феника", 1);
	CHECK_NO_SPELLING(db, "улица Ленинааа", 1);
	CHECK_NO_SPELLING(db, "ууулица Ленина", 1);
	CHECK_NO_SPELLING(db, "улица Линена", 1);

	CHECK_SPELLING(db, "улица Феника", "улица Ленина", 2);
	CHECK_SPELLING(db, "улица Ленинааа", "улица Ленина", 2);
	CHECK_SPELLING(db, "ууулица Ленина", "улица Ленина", 2);
	CHECK_SPELLING(db, "улица Линена", "улица Ленина", 2);

	/* extra cases */
	CHECK_SPELLING(db, "Толстого Льва улица", "улица Льва Толстого", 1);     /* word order */
	CHECK_SPELLING(db, "улица Лебедева Кумача", "улица Лебедева-Кумача", 1); /* word order should not break such typos */
	CHECK_SPELLING(db, "улица Переулок Верхний", "улица Верхний переулок", 1); /* -//- */
	CHECK_SPELLING(db, "улицаленина", "улица Ленина", 1);
	CHECK_SPELLING(db, "зелёнаяулица", "Зелёная улица", 1);

	/* numeric issues */
	CHECK_SPELLING(db, "1-й улица Строителей", "1-я улица Строителей", 1);
	CHECK_NO_SPELLING(db, "2-я улица Строителей", 1);
	CHECK_NO_SPELLING(db, "2-я улица Строителей", 2);

	CHECK_SPELLING(db, "улица -го Интернационала", "улица 3-го Интернационала", 1);

	/*
	 * missing status part
	 */
	CHECK_STRIPPED_STATUS(db, "Ленина");
	CHECK_STRIPPED_STATUS(db, "Зелёная");

	/* names with no status part originally should not be counted
	 * as stripped of status part */
	CHECK_NO_STRIPPED_STATUS(db, "МКАД");

	/*
	 * non-strict status order
	 */
	CHECK_CANONICAL_FORM(db, "р-н Измайловка", "район Измайловка");
	CHECK_CANONICAL_FORM(db, "Измайловка р-н", "район Измайловка");
	/* strictly, following should be CHECK_CANONICAL_FORM, and that should not fail
	 * (e.g. canonical form returning same status part order as in the query. But
	 * that is not implemented yet */
	CHECK_CANONICAL_FORM_HAS(db, "р-н Измайловский", "район Измайловский");
	CHECK_CANONICAL_FORM_HAS(db, "Измайловский р-н", "Измайловский район");

END_TEST()
