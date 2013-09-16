/*
 * Copyright (C) 2011-2013 Dmitry Marakasov
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

Locale::StatusPartDataList status_parts = {
	/* 1 - full form
	 * 2 - canonical form (if nullptr, comes from full form)
	 * 3 - short form (if nullptr, comes from canonical form
	 * 4 - variants (used for detection, so no duplicates are allowed)
	 * 5 - flags */
	/* 1          2        3          4                  5 */
	{ "Avenue",   nullptr, "Ave.",  { "avenue", "ave" }, 0 },
	{ "Close",    nullptr, nullptr, { "close"         }, 0 },
	{ "Court",    nullptr, nullptr, { "court"         }, 0 },
	{ "Crescent", nullptr, nullptr, { "crescent"      }, 0 },
	{ "Gardens",  nullptr, nullptr, { "gardens"       }, 0 },
	{ "Grove",    nullptr, nullptr, { "grove"         }, 0 },
	{ "Lane",     nullptr, nullptr, { "lane"          }, 0 },
	{ "Mews",     nullptr, nullptr, { "mews"          }, 0 },
	{ "Place",    nullptr, nullptr, { "place"         }, 0 },
	{ "Road",     nullptr, "Rd.",   { "road", "rd"    }, 0 },
	{ "Square",   nullptr, "Sq.",   { "square", "sq"  }, 0 },
	{ "Street",   nullptr, "St.",   { "street", "st"  }, 0 },
	{ "Terrace",  nullptr, nullptr, { "terrace"       }, 0 },
	{ "Walk",     nullptr, nullptr, { "walk"          }, 0 },
	{ "Way",      nullptr, nullptr, { "way"           }, 0 },
};

/* register this locale data so it may be used as Locale("en_GB") */
Locale::Registrar registrars[] = {
	Locale::Registrar("en_GB", &status_parts),
};

}
