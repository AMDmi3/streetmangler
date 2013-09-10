/*
 * Copyright (C) 2013 Dmitry Marakasov
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

/* anonymous namespace */
namespace {

using StreetMangler::Locale;

Locale::StatusPartData status_parts[] = {
	/* 1 - full form
	 * 2 - canonical form (if NULL, comes from full form)
	 * 3 - short form (if NULL, comes from canonical form
	 * 4 - variants (used for detection, so no duplicates are allowed)
	 * 5 - flags */
	/* 1             2     3          4                                            5 */
	{ "вулиця",     NULL, "вул.",  { "вулиця", "вул",                      NULL }, 0 },

	{ NULL,         NULL, NULL,    {                                       NULL }, 0 },
};

/* register this locale data so it may be used as Locale("ru_RU") */
Locale::Registrar registrars[] = {
	Locale::Registrar("uk_UA", status_parts),
};

}
