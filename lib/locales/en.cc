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
	{ "Avenue",     NULL, "Ave.",  { "avenue", "ave",                      NULL }, 0 },
	{ "Close",      NULL, NULL,    { "close",                              NULL }, 0 },
	{ "Court",      NULL, NULL,    { "court",                              NULL }, 0 },
	{ "Crescent",   NULL, NULL,    { "crescent",                           NULL }, 0 },
	{ "Gardens",    NULL, NULL,    { "gardens",                            NULL }, 0 },
	{ "Grove",      NULL, NULL,    { "grove",                              NULL }, 0 },
	{ "Lane",       NULL, NULL,    { "lane",                               NULL }, 0 },
	{ "Mews",       NULL, NULL,    { "mews",                               NULL }, 0 },
	{ "Place",      NULL, NULL,    { "place",                              NULL }, 0 },
	{ "Road",       NULL, "Rd.",   { "road", "rd",                         NULL }, 0 },
	{ "Square",     NULL, "Sq.",   { "square", "sq",                       NULL }, 0 },
	{ "Street",     NULL, "St.",   { "street", "st",                       NULL }, 0 },
	{ "Terrace",    NULL, NULL,    { "terrace",                            NULL }, 0 },
	{ "Walk",       NULL, NULL,    { "walk",                               NULL }, 0 },
	{ "Way",        NULL, NULL,    { "way",                                NULL }, 0 },

	{ NULL,         NULL, NULL,    {                                       NULL }, 0 },
};

/* register this locale data so it may be used as Locale("ru_RU") */
Locale::Registrar registrars[] = {
	Locale::Registrar("en_GB", status_parts),
};

}
