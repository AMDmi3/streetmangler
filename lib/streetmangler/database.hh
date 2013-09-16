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

#ifndef STREETMANGLER_DATABASE_HH
#define STREETMANGLER_DATABASE_HH

#include <string>
#include <vector>
#include <memory>

namespace StreetMangler {

class Locale;
class Name;

class Database {
public:
	Database(const Locale& locale);
	virtual ~Database();

	void Load(const std::string& filename);
	void Add(const std::string& name);

	const Locale& GetLocale() const;

	int CheckExactMatch(const std::string& name) const;
	int CheckCanonicalForm(const std::string& name, std::vector<std::string>& suggestions) const;
	int CheckSpelling(const std::string& name, std::vector<std::string>& suggestions, int depth = 1) const;
	int CheckStrippedStatus(const std::string& name, std::vector<std::string>& matches) const;

	int CheckExactMatch(const Name& name) const;
	int CheckCanonicalForm(const Name& name, std::vector<std::string>& suggestions) const;
	int CheckSpelling(const Name& name, std::vector<std::string>& suggestions, int depth = 1) const;
	int CheckStrippedStatus(const Name& name, std::vector<std::string>& matches) const;

private:
	class Private;
	std::auto_ptr<Private> private_;
};

}

#endif
