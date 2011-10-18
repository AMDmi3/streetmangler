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

#include <streetmangler/name.hh>
#include "testing.hh"

BEGIN_TEST()
	using StreetMangler::Locale;
	using StreetMangler::Name;

	/* assumes working locale, see locale_test */
	Locale locale("ru_RU");

	/* passthrough: should not change string without flags  */
	EXPECT_STRING(Name("улица Ленина", locale).Join(), "улица Ленина");
	EXPECT_STRING(Name("    улица    Ленина   ", locale).Join(), "    улица    Ленина   ");
	EXPECT_STRING(Name(" . , \t улица.,\t  .,\t Ленина .,\t  ", locale).Join(), " . , \t улица.,\t  .,\t Ленина .,\t  ");

	/* basic flags: normalize whitespace */
	EXPECT_STRING(Name(" . , \t улица.,\t  .,\t Ленина .,\t  ", locale).Join(Name::NORMALIZE_WHITESPACE), ". , улица., ., Ленина .,");

	/* basic flags: normalize punctiation */
	EXPECT_STRING(Name("Зелёная,улица", locale).Join(Name::NORMALIZE_PUNCT), "Зелёная улица");
	EXPECT_STRING(Name("Зелёная, улица", locale).Join(Name::NORMALIZE_PUNCT), "Зелёная улица");
	EXPECT_STRING(Name("Зелёная,  улица", locale).Join(Name::NORMALIZE_PUNCT), "Зелёная  улица");

	/* some combinations */
	EXPECT_STRING(Name("Зелёная,  улица", locale).Join(Name::NORMALIZE_PUNCT|Name::NORMALIZE_WHITESPACE), "Зелёная улица");
	EXPECT_STRING(Name("Зелёная , улица", locale).Join(Name::NORMALIZE_PUNCT|Name::NORMALIZE_WHITESPACE), "Зелёная улица");
	EXPECT_STRING(Name("Зелёная  ,улица", locale).Join(Name::NORMALIZE_PUNCT|Name::NORMALIZE_WHITESPACE), "Зелёная улица");

	/* basic flags: status modes */
	EXPECT_STRING(Name("улица Ленина", locale).Join(Name::EXPAND_STATUS), "улица Ленина");
	EXPECT_STRING(Name("ул. Ленина", locale).Join(Name::EXPAND_STATUS), "улица Ленина");
	EXPECT_STRING(Name("ул.Ленина", locale).Join(Name::EXPAND_STATUS), "улица Ленина");
	EXPECT_STRING(Name("ул Ленина", locale).Join(Name::EXPAND_STATUS), "улица Ленина");
	EXPECT_STRING(Name("Зелёная ул", locale).Join(Name::EXPAND_STATUS), "Зелёная улица");
	EXPECT_STRING(Name("Зелёная ул.", locale).Join(Name::EXPAND_STATUS), "Зелёная улица");
	EXPECT_STRING(Name("Зелёная улица", locale).Join(Name::EXPAND_STATUS), "Зелёная улица");
	EXPECT_STRING(Name("Зелёная,ул", locale).Join(Name::EXPAND_STATUS), "Зелёная,улица");
	EXPECT_STRING(Name("Зелёная,ул.", locale).Join(Name::EXPAND_STATUS), "Зелёная,улица");
	EXPECT_STRING(Name("Зелёная,улица", locale).Join(Name::EXPAND_STATUS), "Зелёная,улица");
	EXPECT_STRING(Name("Зелёная, ул", locale).Join(Name::EXPAND_STATUS), "Зелёная, улица");
	EXPECT_STRING(Name("Зелёная, ул.", locale).Join(Name::EXPAND_STATUS), "Зелёная, улица");
	EXPECT_STRING(Name("Зелёная, улица", locale).Join(Name::EXPAND_STATUS), "Зелёная, улица");

	EXPECT_STRING(Name("улица Ленина", locale).Join(Name::SHRINK_STATUS), "ул. Ленина");
	EXPECT_STRING(Name("ул. Ленина", locale).Join(Name::SHRINK_STATUS), "ул. Ленина");
	EXPECT_STRING(Name("ул.Ленина", locale).Join(Name::SHRINK_STATUS), "ул. Ленина");
	EXPECT_STRING(Name("ул Ленина", locale).Join(Name::SHRINK_STATUS), "ул. Ленина");
	EXPECT_STRING(Name("Зелёная ул", locale).Join(Name::SHRINK_STATUS), "Зелёная ул.");
	EXPECT_STRING(Name("Зелёная ул.", locale).Join(Name::SHRINK_STATUS), "Зелёная ул.");
	EXPECT_STRING(Name("Зелёная улица", locale).Join(Name::SHRINK_STATUS), "Зелёная ул.");
	EXPECT_STRING(Name("Зелёная,ул", locale).Join(Name::SHRINK_STATUS), "Зелёная,ул.");
	EXPECT_STRING(Name("Зелёная,ул.", locale).Join(Name::SHRINK_STATUS), "Зелёная,ул.");
	EXPECT_STRING(Name("Зелёная,улица", locale).Join(Name::SHRINK_STATUS), "Зелёная,ул.");
	EXPECT_STRING(Name("Зелёная, ул", locale).Join(Name::SHRINK_STATUS), "Зелёная, ул.");
	EXPECT_STRING(Name("Зелёная, ул.", locale).Join(Name::SHRINK_STATUS), "Зелёная, ул.");
	EXPECT_STRING(Name("Зелёная, улица", locale).Join(Name::SHRINK_STATUS), "Зелёная, ул.");

	/* basic flags: status order */
	EXPECT_STRING(Name("Зелёная улица", locale).Join(Name::STATUS_TO_LEFT), "улица Зелёная");
	EXPECT_STRING(Name("Зелёная, улица", locale).Join(Name::STATUS_TO_LEFT), "улица Зелёная");
	EXPECT_STRING(Name("улица Ленина", locale).Join(Name::STATUS_TO_LEFT), "улица Ленина");
	EXPECT_STRING(Name("1-я улица Доватора", locale).Join(Name::STATUS_TO_LEFT), "улица 1-я Доватора");

	EXPECT_STRING(Name("Зелёная улица", locale).Join(Name::STATUS_TO_RIGHT), "Зелёная улица");
	EXPECT_STRING(Name("Зелёная, улица", locale).Join(Name::STATUS_TO_RIGHT), "Зелёная улица");
	EXPECT_STRING(Name("улица Ленина", locale).Join(Name::STATUS_TO_RIGHT), "Ленина улица");
	EXPECT_STRING(Name("1-я улица Доватора", locale).Join(Name::STATUS_TO_RIGHT), "1-я Доватора улица");

	EXPECT_STRING(Name("улица", locale).Join(Name::STATUS_TO_LEFT), "улица");
	EXPECT_STRING(Name("улица", locale).Join(Name::STATUS_TO_RIGHT), "улица");

	/* basic flags: removing all statuses */
	EXPECT_STRING(Name("улица Широкое Шоссе", locale).Join(Name::REMOVE_ALL_STATUSES), "Широкое");
	EXPECT_STRING(Name("  улица\tулица улица", locale).Join(Name::REMOVE_ALL_STATUSES), "");
	EXPECT_STRING(Name("Зелёная,  ул.", locale).Join(Name::REMOVE_ALL_STATUSES), "Зелёная");

	/* whitespace/punct normalization */
	EXPECT_STRING(Name("Зелёная,  ул.", locale).Join(Name::NORMALIZE_PUNCT), "Зелёная  ул.");
	EXPECT_STRING(Name("Зелёная,  ул.", locale).Join(Name::NORMALIZE_WHITESPACE), "Зелёная, ул.");
	EXPECT_STRING(Name("Зелёная,  ул.", locale).Join(Name::NORMALIZE_PUNCT|Name::NORMALIZE_WHITESPACE), "Зелёная ул.");

	/* does it work with no status part? */
	EXPECT_STRING(Name("Зелёная Штуковина", locale).Join(Name::EXPAND_STATUS), "Зелёная Штуковина");
	EXPECT_STRING(Name("Зелёная Штуковина", locale).Join(Name::SHRINK_STATUS), "Зелёная Штуковина");
	EXPECT_STRING(Name("Зелёная Штуковина", locale).Join(Name::REMOVE_ALL_STATUSES), "Зелёная Штуковина");
	EXPECT_STRING(Name("Зелёная Штуковина", locale).Join(Name::STATUS_TO_LEFT), "Зелёная Штуковина");
	EXPECT_STRING(Name("Зелёная Штуковина", locale).Join(Name::STATUS_TO_RIGHT), "Зелёная Штуковина");
	EXPECT_STRING(Name("Зелёная  Штуковина", locale).Join(), "Зелёная  Штуковина");
	EXPECT_STRING(Name("Зелёная  Штуковина", locale).Join(Name::NORMALIZE_WHITESPACE), "Зелёная Штуковина");

	/* big matrix */
	const char* inputs[] = {
		"Ул Ленина",
		"ул Ленина",
		"ул.Ленина",
		"ул. Ленина",
		"ул.  Ленина",
		" ул.  Ленина ",
		"улица Ленина",
		"улица  Ленина",
		"  улица  Ленина  ",
		"Ленина ул",
		"Ленина ул.",
		"Ленина,ул",
		"Ленина,ул.",
		"Ленина,улица",
		"Ленина, ул",
		"Ленина, ул.",
		"Ленина, улица",
		"Ленина, УЛИЦА",
		"Ленина улица",
		"УЛИЦА Ленина",
		"   улица   Ленина   ",
		"   Ленина   улица   ",
		"\tулица\tЛенина\t",
		"\tЛенина\tулица\t",
	};

	struct Output {
		int flags;
		const char* expected;
	} outputs[] = {
		{ Name::EXPAND_STATUS | Name::STATUS_TO_LEFT | Name::NORMALIZE_WHITESPACE, "улица Ленина" },
		{ Name::EXPAND_STATUS | Name::STATUS_TO_RIGHT | Name::NORMALIZE_WHITESPACE, "Ленина улица" },
		{ Name::SHRINK_STATUS | Name::STATUS_TO_LEFT | Name::NORMALIZE_WHITESPACE, "ул. Ленина" },
		{ Name::SHRINK_STATUS | Name::STATUS_TO_RIGHT | Name::NORMALIZE_WHITESPACE, "Ленина ул." },
	};

	for (const char** input = inputs; input != inputs + sizeof(inputs)/sizeof(inputs[0]); ++input)
		for (Output* output = outputs; output != outputs + sizeof(outputs)/sizeof(outputs[0]); ++output)
			EXPECT_STRING(Name(*input, locale).Join(output->flags), output->expected);

END_TEST()
