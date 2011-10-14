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
	/* canonical full form
	   |             generic canonical form
	   |             |     canonical abbreviated form
	   |             |     |          variants
	   |             |     |          |                                         */
	{ "улица",      NULL, "ул.",   { "улица", "ул",                        NULL } },

	{ "площадь",    NULL, "пл.",   { "площадь", "пл",                      NULL } },
	{ "проезд",     NULL, "пр.",   { "проезд", "пр-д",                     NULL } },
	{ "переулок",   NULL, "пер.",  { "переулок", "пер", "пер-к",           NULL } },
	{ "шоссе",      NULL, "ш.",    { "шоссе", "ш",                         NULL } },

	{ "бульвар",    NULL, "бул.",  { "бульвар", "бул", "б-р",              NULL } },
	{ "тупик",      NULL, "туп.",  { "тупик", "туп",                       NULL } },
	{ "набережная", NULL, "наб.",  { "набережная", "наб",                  NULL } },
	{ "проспект",   NULL, "пр-т.", { "проспект", "просп", "пр-кт", "пр-т", NULL } },
	{ "линия",      NULL, NULL,    { "линия",                              NULL } },
	{ "аллея",      NULL, NULL,    { "аллея",                              NULL } },

	{ "метромост",  NULL, NULL,    { "метромост",                          NULL } },
	{ "мост",       NULL, NULL,    { "мост",                               NULL } },
	{ "просек",     NULL, NULL,    { "просек",                             NULL } },
	{ "просека",    NULL, NULL,    { "просека",                            NULL } },
	{ "путепровод", NULL, NULL,    { "путепровод",                         NULL } },
	/* после шоссе т.е. "шоссе ***й Тракт" */
	{ "тракт",      NULL, NULL,    { "тракт", "тр-т", "тр",                NULL } },
	{ "тропа",      NULL, NULL,    { "тропа",                              NULL } },
	{ "туннель",    NULL, NULL,    { "туннель",                            NULL } },
	{ "тоннель",    NULL, NULL,    { "тоннель",                            NULL } },
	{ "эстакада",   NULL, NULL,    { "эстакада", "эст",                    NULL } },
	{ "дорога",     NULL, "дор.",  { "дорога", "дор",                      NULL } },

	{ "спуск",      NULL, NULL,    { "спуск",                              NULL } },
	{ "подход",     NULL, NULL,    { "подход",                             NULL } },
	{ "подъезд",    NULL, NULL,    { "подъезд",                            NULL } },
	{ "съезд",      NULL, NULL,    { "съезд",                              NULL } },
	{ "заезд",      NULL, NULL,    { "заезд",                              NULL } },
	{ "разъезд",    NULL, NULL,    { "разъезд",                            NULL } },

	{ NULL,         NULL, NULL,    {                                       NULL } },
};

/* register this locale data so it may be created as Locale("ru") */
Locale::Registrar registrars[] = {
	Locale::Registrar("ru", status_parts),
	Locale::Registrar("ru_RU", status_parts),
};

};
