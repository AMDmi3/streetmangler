/*
 * Copyright (C) 2016 Dmitry Marakasov
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
#include "testing.hh"

namespace {

StreetMangler::Locale::StatusPartDataList status_parts_ok = {
	{ "улица", nullptr, "ул.", { "улица", "ул" }, 0 },
};

StreetMangler::Locale::StatusPartDataList status_parts_dup = {
	{ "улица", nullptr, "ул.", { "улица", "ул" }, 0 },
	{ "улица", nullptr, "ул.", { "улица", "ул" }, 0 },
};

StreetMangler::Locale::StatusPartDataList status_parts_nofull = {
	{ nullptr, nullptr, "ул.", { "улица", "ул" }, 0 },
};

}

BEGIN_TEST()
	using namespace StreetMangler;

	// Note: registrars are designed to be static, never be destroyed in
	// runtime and never be created on stack; here we do it only for testing
	// purposes, you should never do it in real code
	Locale::Registrar r_ok("ok", &status_parts_ok);

	// Check for duplicate locale name
	EXPECT_EXCEPTION(Locale::Registrar("ok", &status_parts_ok), Locale::BadLocale);

	// Throw on duplicate locale entry
	Locale::Registrar r_dup("dup", &status_parts_dup);
	EXPECT_EXCEPTION(Locale("dup"), Locale::BadLocale);

	// Throw on locale entry without full name
	Locale::Registrar r_nofull("nofull", &status_parts_nofull);
	EXPECT_EXCEPTION(Locale("nofull"), Locale::BadLocale);

	// Locale exceptions
	try {
		throw Locale::BadLocale("BadLocaleString");
	} catch (std::exception& e) {
		EXPECT_TRUE(std::string(e.what()) == "BadLocaleString");
	}

	try {
		throw Locale::UnknownLocale("UnknownLocaleString");
	} catch (std::exception& e) {
		EXPECT_TRUE(std::string(e.what()) == "UnknownLocaleString");
	}
END_TEST()
