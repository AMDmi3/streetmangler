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
	/* 1            2        3          4                                       5 */
	{ "улица",      nullptr, "ул.",   { "улица", "ул"                        }, 0 },

	{ "переулок",   nullptr, "пер.",  { "переулок", "пер", "пер-к"           }, 0 },
	/* после переулка, т.к. "переулок Одесский проезд" */
	{ "проезд",     nullptr, "пр-д.", { "проезд", "пр-д"                     }, 0 },
	/* после переулка, т.к. "переулок Пролетарская Площадь" */
	{ "площадь",    nullptr, "пл.",   { "площадь", "пл"                      }, 0 },
	{ "шоссе",      nullptr, "ш.",    { "шоссе", "ш"                         }, 0 },

	{ "бульвар",    nullptr, "бул.",  { "бульвар", "бул", "б-р"              }, 0 },
	{ "тупик",      nullptr, "туп.",  { "тупик", "туп"                       }, 0 },
	{ "набережная", nullptr, "наб.",  { "набережная", "наб"                  }, 0 },
	{ "проспект",   nullptr, "пр-т.", { "проспект", "просп", "пр-кт", "пр-т" }, 0 },
	{ "линия",      nullptr, nullptr, { "линия"                              }, 0 },
	{ "аллея",      nullptr, nullptr, { "аллея"                              }, 0 },

	{ "метромост",  nullptr, nullptr, { "метромост"                          }, 0 },
	{ "мост",       nullptr, nullptr, { "мост"                               }, 0 },
	{ "просек",     nullptr, nullptr, { "просек"                             }, 0 },
	{ "просека",    nullptr, nullptr, { "просека"                            }, 0 },
	{ "путепровод", nullptr, nullptr, { "путепровод"                         }, 0 },
	/* после шоссе т.к. "шоссе ***й Тракт" */
	{ "тракт",      nullptr, nullptr, { "тракт", "тр-т", "тр"                }, 0 },
	{ "тропа",      nullptr, nullptr, { "тропа"                              }, 0 },
	{ "туннель",    nullptr, nullptr, { "туннель"                            }, 0 },
	{ "тоннель",    nullptr, nullptr, { "тоннель"                            }, 0 },
	{ "эстакада",   nullptr, nullptr, { "эстакада", "эст"                    }, 0 },
//	{ "дорога",     nullptr, "дор.",  { "дорога", "дор"                      }, true },*/

	{ "спуск",      nullptr, nullptr, { "спуск"                              }, 0 },
	{ "подход",     nullptr, nullptr, { "подход"                             }, 0 },
//	{ "подъезд",    nullptr, nullptr, { "подъезд"                            }, 0 },*/
	{ "съезд",      nullptr, nullptr, { "съезд"                              }, 0 },
	{ "заезд",      nullptr, nullptr, { "заезд"                              }, 0 },
	{ "разъезд",    nullptr, nullptr, { "разъезд"                            }, 0 },
	{ "слобода",    nullptr, nullptr, { "слобода"                            }, 0 },

	{ "район",      nullptr, nullptr, { "район", "р-н"                       }, Locale::ORDER_RANDOM_IF_RIGHT },
	{ "микрорайон", nullptr, nullptr, { "микрорайон", "мкр-н", "мк-н", "мкр-он", "мкр", "мкрн" }, Locale::ORDER_RANDOM_IF_RIGHT },
	{ "посёлок",    nullptr, nullptr, { "посёлок", "поселок", "пос"          }, Locale::ORDER_RANDOM_IF_RIGHT },
	{ "деревня",    nullptr, nullptr, { "деревня", "дер", "д"                }, Locale::ORDER_RANDOM_IF_RIGHT },
	{ "квартал",    nullptr, nullptr, { "квартал", "кв-л", "кв"              }, Locale::ORDER_RANDOM_IF_RIGHT },
};

/* register this locale data so it may be used as Locale("ru_RU") */
Locale::Registrar registrars[] = {
	Locale::Registrar("ru_RU", &status_parts),
};

}
