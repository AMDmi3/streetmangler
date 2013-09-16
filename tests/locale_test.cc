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
#include "testing.hh"

BEGIN_TEST()
	using namespace StreetMangler;

	/* attempt to use nonexistent locale throws an exception */
	EXPECT_EXCEPTION(Locale locale("NONEXISTENT"), Locale::UnknownLocale);

	/* using builtin locale whould not throw */
	EXPECT_NO_EXCEPTION(Locale locale("ru_RU"));

	Locale locale("ru_RU");

	const Locale::StatusPart* p;
	const Locale::StatusPart* p1;
	const Locale::StatusPart* p2;

	/* finding status part should work */
	EXPECT_TRUE((p = locale.FindStatus("улица")) != nullptr);
	EXPECT_TRUE(locale.FindStatus("ул") == p);

	/* finding nonexistent status parts should not work */
	EXPECT_TRUE(locale.FindStatus("неттакогосттуса") == nullptr);
	EXPECT_TRUE(locale.FindStatus("у") == nullptr);
	EXPECT_TRUE(locale.FindStatus("") == nullptr);

	/* correct values returned */
	EXPECT_TRUE(p->GetFull() == "улица");
	EXPECT_TRUE(p->GetAbbrev() == "ул.");

	/* correct ordering */
	EXPECT_TRUE((p1 = locale.FindStatus("шоссе")) != nullptr);
	EXPECT_TRUE((p2 = locale.FindStatus("тракт")) != nullptr);

	EXPECT_TRUE(p1->IsPrior(p2));
	EXPECT_TRUE(p1->IsPrior(nullptr));
END_TEST()
