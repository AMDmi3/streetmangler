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

#ifndef STREETMANGLER_DATABASE_HH
#define STREETMANGLER_DATABASE_HH

#include <set>
#include <string>
#include <vector>
#include <map>

#include <tspell/unitrie.hh>

namespace StreetMangler {

class Locale;
class Name;

class Database {
public:
	Database(const Locale& locale);
	virtual ~Database();
	void Load(const char* filename);
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
	void NameToHash(const Name& name, std::string& hash) const;
	void NameToUnicodeHash(const Name& name, UnicodeString& hash) const;

private:
	typedef std::set<std::string> NamesSet;
	typedef std::multimap<std::string, std::string> NamesMap;

private:
	const Locale& locale_;
	NamesSet names_;
	NamesMap canonical_map_;
	NamesMap stripped_map_;

	TSpell::UnicodeTrie spell_trie_;
};

}

#endif
