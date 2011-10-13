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
	/* full status
	   |             canonical abbreviation
	   |             |          possible abbreviations
	   |             |          |                                    */
	{ "улица",      "ул.",   { "ул",                           NULL } },

	{ "площадь",    "пл.",   { "пл",                           NULL } },
	{ "проезд",     "пр.",   { "пр-д",                         NULL } },
	{ "переулок",   "пер.",  { "пер", "пер-к",                 NULL } },
	{ "шоссе",      "ш.",    { "ш",                            NULL } },

	{ "бульвар",    "бул.",  { "бул", "б-р",                   NULL } },
	{ "тупик",      "туп.",  { "туп",                          NULL } },
	{ "набережная", "наб.",  { "наб",                          NULL } },
	{ "проспект",   "пр-т.", { "просп", "пр-кт", "пр-т",       NULL } },
	{ "линия",      NULL,    {                                 NULL } },
	{ "аллея",      NULL,    {                                 NULL } },

	{ "метромост",  NULL,    {                                 NULL } },
	{ "мост",       NULL,    {                                 NULL } },
	{ "просек",     NULL,    {                                 NULL } },
	{ "просека",    NULL,    {                                 NULL } },
	{ "путепровод", NULL,    {                                 NULL } },
	/* после шоссе т.е. "шоссе ***й Тракт" */
	{ "тракт",      NULL,    { "тр-т", "тр",                   NULL } },
	{ "тропа",      NULL,    {                                 NULL } },
	{ "туннель",    NULL,    {                                 NULL } },
	{ "тоннель",    NULL,    {                                 NULL } },
	{ "эстакада",   NULL,    { "эст",                          NULL } },
	{ "дорога",     "дор.",  { "дор",                          NULL } },

	{ "спуск",      NULL,    {                                 NULL } },
	{ "подход",     NULL,    {                                 NULL } },
	{ "подъезд",    NULL,    {                                 NULL } },
	{ "съезд",      NULL,    {                                 NULL } },
	{ "заезд",      NULL,    {                                 NULL } },
	{ "разъезд",    NULL,    {                                 NULL } },

	{ NULL,         NULL,    {                                 NULL } },
};

/* register this locale data so it may be created as Locale("ru") */
Locale::Registrar registrars[] = {
	Locale::Registrar("ru", status_parts),
	/* TODO: maybe use ru_RU notation? */
};

};
