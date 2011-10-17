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

#include <streetmangler/locale.hh>
#include <streetmangler/database.hh>
#include "database_testing.hh"

using namespace StreetMangler;

namespace {
	/* locale with custom (in this case, abbreviated) canonical form */
	StatusPartData status_parts[] = {
		{ "ул.", "улица", "ул.", { "улица", "ул", NULL } },
		{ "просп.", "проспект", "пр.", { "проспект", "просп", "пр", NULL } },
		{ NULL, NULL, NULL, { NULL } },
	};

	Locale::Registrar registrars[] = {
		Locale::Registrar("test", status_parts),
	};
}

BEGIN_TEST()
	Locale locale("test");

	Database db(locale);

	/* database always stores full form */
	db.Add("проспект Ленина");
	db.Add("Зелёная улица");

	/* ExactMatch should match with canonical form */
	CHECK_NO_EXACT_MATCH(db, "проспект Ленина");
	CHECK_EXACT_MATCH(db, "просп. Ленина");
	CHECK_NO_EXACT_MATCH(db, "пр. Ленина");

	CHECK_NO_EXACT_MATCH(db, "Зелёная улица");
	CHECK_EXACT_MATCH(db, "Зелёная ул.");

	/* CanonicalForm returns canonical form */
	CHECK_CANONICAL_FORM(db, "проспект Ленина", "просп. Ленина");
	CHECK_CANONICAL_FORM(db, "пр. Ленина", "просп. Ленина");
	CHECK_CANONICAL_FORM(db, "просп. Ленина", "просп. Ленина");

	CHECK_CANONICAL_FORM(db, "ул.Зелёная", "Зелёная ул.");
	CHECK_CANONICAL_FORM(db, "Зелёная,улица", "Зелёная ул.");

	/* StrippedStatus should still work */
	CHECK_STRIPPED_STATUS(db, "Ленина");
	CHECK_STRIPPED_STATUS(db, "Зелёная");
	CHECK_NO_STRIPPED_STATUS(db, "Красная");

END_TEST()
