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
	 * 5 - whether status part ordering is strict */
	/* 1             2     3           4                                            5 */
	{ "улица",      NULL, "ул.",   { "улица", "ул",                        NULL }, true },

	{ "площадь",    NULL, "пл.",   { "площадь", "пл",                      NULL }, true },
	{ "переулок",   NULL, "пер.",  { "переулок", "пер", "пер-к",           NULL }, true },
	/* после переулка, т.е. "переулок Одесский проезд" */
	{ "проезд",     NULL, "пр-д.", { "проезд", "пр-д",                     NULL }, true },
	{ "шоссе",      NULL, "ш.",    { "шоссе", "ш",                         NULL }, true },

	{ "бульвар",    NULL, "бул.",  { "бульвар", "бул", "б-р",              NULL }, true },
	{ "тупик",      NULL, "туп.",  { "тупик", "туп",                       NULL }, true },
	{ "набережная", NULL, "наб.",  { "набережная", "наб",                  NULL }, true },
	{ "проспект",   NULL, "пр-т.", { "проспект", "просп", "пр-кт", "пр-т", NULL }, true },
	{ "линия",      NULL, NULL,    { "линия",                              NULL }, true },
	{ "аллея",      NULL, NULL,    { "аллея",                              NULL }, true },

	{ "метромост",  NULL, NULL,    { "метромост",                          NULL }, true },
	{ "мост",       NULL, NULL,    { "мост",                               NULL }, true },
	{ "просек",     NULL, NULL,    { "просек",                             NULL }, true },
	{ "просека",    NULL, NULL,    { "просека",                            NULL }, true },
	{ "путепровод", NULL, NULL,    { "путепровод",                         NULL }, true },
	/* после шоссе т.к. "шоссе ***й Тракт" */
	{ "тракт",      NULL, NULL,    { "тракт", "тр-т", "тр",                NULL }, true },
	{ "тропа",      NULL, NULL,    { "тропа",                              NULL }, true },
	{ "туннель",    NULL, NULL,    { "туннель",                            NULL }, true },
	{ "тоннель",    NULL, NULL,    { "тоннель",                            NULL }, true },
	{ "эстакада",   NULL, NULL,    { "эстакада", "эст",                    NULL }, true },
	/*{ "дорога",     NULL, "дор.",  { "дорога", "дор",                      NULL }, true },*/

	{ "спуск",      NULL, NULL,    { "спуск",                              NULL }, true },
	{ "подход",     NULL, NULL,    { "подход",                             NULL }, true },
	{ "подъезд",    NULL, NULL,    { "подъезд",                            NULL }, true },
	{ "съезд",      NULL, NULL,    { "съезд",                              NULL }, true },
	{ "заезд",      NULL, NULL,    { "заезд",                              NULL }, true },
	{ "разъезд",    NULL, NULL,    { "разъезд",                            NULL }, true },
	{ "слобода",    NULL, NULL,    { "слобода",                            NULL }, true },

	{ "район",      NULL, NULL,    { "район", "р-н",                       NULL }, false },
	{ "микрорайон", NULL, NULL,    { "микрорайон", "мкр-н", "мк-н", "мкр", "мкрн", NULL }, false },
	{ "посёлок",    NULL, NULL,    { "посёлок", "поселок", "пос",          NULL }, false },
	{ "деревня",    NULL, NULL,    { "деревня", "дер", "д",                NULL }, false },
	{ "квартал",    NULL, NULL,    { "квартал", "кв-л",                    NULL }, false },

	{ NULL,         NULL, NULL,    {                                       NULL }, true },
};

/* register this locale data so it may be used as Locale("ru_RU") */
Locale::Registrar registrars[] = {
	Locale::Registrar("ru_RU", status_parts),
};

};
