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

#include <stdexcept>

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <streetmangler/stringlistparser.hh>

namespace StreetMangler {

StringListParser::StringListParser(const std::string& filename) : filename_(filename) {
}

StringListParser::~StringListParser() {
}

void StringListParser::Parse() {
	int f;
	if ((f = open(filename_.c_str(), O_RDONLY)) == -1)
		throw std::runtime_error(std::string("Cannot open ") + filename_ + ": " + strerror(errno));

	char buffer[1024];
	ssize_t nread;

	std::string name;
	int line = 1;
	bool space = false;
	bool comment = false;
	while ((nread = read(f, buffer, sizeof(buffer))) > 0) {
		for (char* cur = buffer; cur != buffer + nread; ++cur) {
			if (*cur == '\n') {
				if (!name.empty()) {
					ProcessString(name);
					name.clear();
				}
				line++;
				comment = false;
				space = false;
			} else if (comment) {
				/* skip until eol */
			} else if (*cur == '#') {
				comment = true;
			} else if (*cur == ' ' || *cur == '\t') {
				space = true;
			} else {
				if (space && !name.empty()) {
					name += ' ';
					space = false;
				}
				name += *cur;
			}
		}
	}

	if (!name.empty())
		ProcessString(name);

	if (nread == -1)
		throw std::runtime_error(std::string("Read error on ") + filename_ + ": " + strerror(errno));

	close(f);
}

}
