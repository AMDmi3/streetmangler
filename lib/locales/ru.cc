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

/* anonymous namespace */
namespace {

using namespace StreetMangler;

StatusPartData status_parts[] = {
	/* 1 - full form
	 * 2 - canonical form (if NULL, comes from full form)
	 * 3 - short form (if NULL, comes from canonical form
	 * 4 - variants (used for detection, so no duplicates are allowed)
	 * 5 - flags */
	/* 1             2     3           4                                            5 */
	{ "улица",      NULL, "ул.",   { "улица", "ул",                        NULL }, 0 },

	{ "площадь",    NULL, "пл.",   { "площадь", "пл",                      NULL }, 0 },
	{ "переулок",   NULL, "пер.",  { "переулок", "пер", "пер-к",           NULL }, 0 },
	/* после переулка, т.е. "переулок Одесский проезд" */
	{ "проезд",     NULL, "пр-д.", { "проезд", "пр-д",                     NULL }, 0 },
	{ "шоссе",      NULL, "ш.",    { "шоссе", "ш",                         NULL }, 0 },

	{ "бульвар",    NULL, "бул.",  { "бульвар", "бул", "б-р",              NULL }, 0 },
	{ "тупик",      NULL, "туп.",  { "тупик", "туп",                       NULL }, 0 },
	{ "набережная", NULL, "наб.",  { "набережная", "наб",                  NULL }, 0 },
	{ "проспект",   NULL, "пр-т.", { "проспект", "просп", "пр-кт", "пр-т", NULL }, 0 },
	{ "линия",      NULL, NULL,    { "линия",                              NULL }, 0 },
	{ "аллея",      NULL, NULL,    { "аллея",                              NULL }, 0 },

	{ "метромост",  NULL, NULL,    { "метромост",                          NULL }, 0 },
	{ "мост",       NULL, NULL,    { "мост",                               NULL }, 0 },
	{ "просек",     NULL, NULL,    { "просек",                             NULL }, 0 },
	{ "просека",    NULL, NULL,    { "просека",                            NULL }, 0 },
	{ "путепровод", NULL, NULL,    { "путепровод",                         NULL }, 0 },
	/* после шоссе т.к. "шоссе ***й Тракт" */
	{ "тракт",      NULL, NULL,    { "тракт", "тр-т", "тр",                NULL }, 0 },
	{ "тропа",      NULL, NULL,    { "тропа",                              NULL }, 0 },
	{ "туннель",    NULL, NULL,    { "туннель",                            NULL }, 0 },
	{ "тоннель",    NULL, NULL,    { "тоннель",                            NULL }, 0 },
	{ "эстакада",   NULL, NULL,    { "эстакада", "эст",                    NULL }, 0 },
	/*{ "дорога",     NULL, "дор.",  { "дорога", "дор",                      NULL }, true },*/

	{ "спуск",      NULL, NULL,    { "спуск",                              NULL }, 0 },
	{ "подход",     NULL, NULL,    { "подход",                             NULL }, 0 },
	{ "подъезд",    NULL, NULL,    { "подъезд",                            NULL }, 0 },
	{ "съезд",      NULL, NULL,    { "съезд",                              NULL }, 0 },
	{ "заезд",      NULL, NULL,    { "заезд",                              NULL }, 0 },
	{ "разъезд",    NULL, NULL,    { "разъезд",                            NULL }, 0 },
	{ "слобода",    NULL, NULL,    { "слобода",                            NULL }, 0 },

	{ "район",      NULL, NULL,    { "район", "р-н",                       NULL }, RANDOM_ORDER },
	{ "микрорайон", NULL, NULL,    { "микрорайон", "мкр-н", "мк-н", "мкр", "мкрн", NULL }, RANDOM_ORDER },
	{ "посёлок",    NULL, NULL,    { "посёлок", "поселок", "пос",          NULL }, RANDOM_ORDER },
	{ "деревня",    NULL, NULL,    { "деревня", "дер", "д",                NULL }, RANDOM_ORDER },
	{ "квартал",    NULL, NULL,    { "квартал", "кв-л",                    NULL }, RANDOM_ORDER },

	{ NULL,         NULL, NULL,    {                                       NULL }, 0 },
};

/* register this locale data so it may be used as Locale("ru_RU") */
Locale::Registrar registrars[] = {
	Locale::Registrar("ru_RU", status_parts),
};

};
