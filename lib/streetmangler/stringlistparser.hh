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

#ifndef STREETMANGLER_STRINGLISTPARSER_HH
#define STREETMANGLER_STRINGLISTPARSER_HH

#include <string>

namespace StreetMangler {

class StringListParser {
public:
	StringListParser();
	virtual ~StringListParser();

	void ParseFile(const char* filename);

protected:
	virtual void ProcessString(const std::string& string) = 0;
};

}

#endif
