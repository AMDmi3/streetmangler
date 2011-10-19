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

#include <set>
#include <string>
#include <stdexcept>
#include <vector>

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <unicode/unistr.h>

#include <streetmangler/database.hh>
#include <streetmangler/locale.hh>
#include <streetmangler/name.hh>

namespace StreetMangler {

Database::Database(const Locale& locale) : locale_(locale) {
}

Database::~Database() {
}

void Database::Load(const char* filename) {
	int f;
	if ((f = open(filename, O_RDONLY)) == -1)
		throw std::runtime_error(std::string("Cannot open database: ") + strerror(errno));

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
					Add(name);
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
		Add(name);

	if (nread == -1)
		throw std::runtime_error(std::string("Read error: ") + strerror(errno));

	close(f);
}

const StreetMangler::Locale& StreetMangler::Database::GetLocale() const {
	return locale_;
}

void Database::NameToHash(const Name& name, std::string& hash) const {
	UnicodeString temp;
	NameToUnicodeHash(name, temp);
	temp.toUTF8String(hash);
}

void Database::NameToUnicodeHash(const Name& name, UnicodeString& hash) const {
	static const int flags = Name::STATUS_TO_LEFT | Name::EXPAND_STATUS | Name::NORMALIZE_WHITESPACE | Name::NORMALIZE_PUNCT;
	hash = UnicodeString::fromUTF8(name.Join(flags)).toLower();
}

void Database::Add(const std::string& name) {
	Name tokenized(name, locale_);

	std::string hash;
	UnicodeString uhash;
	NameToUnicodeHash(tokenized, uhash);
	uhash.toUTF8String(hash);

	/* for the locales in which canonical form != full form,
	 * we need to use canonical form as a reference */
	std::string canonical = tokenized.Join(Name::CANONICALIZE_STATUS);

	/* for exact match */
	names_.insert(canonical);

	/* for canonical form  */
	canonical_map_.insert(std::make_pair(hash, canonical));

	/* for spelling */
	spell_trie_.Insert(uhash);

	/* for stripped status  */
	std::string stripped = tokenized.Join(Name::REMOVE_ALL_STATUSES);
	if (stripped != name)
		stripped_map_.insert(std::make_pair(stripped, canonical));
}

/*
 * Checks
 */
int Database::CheckExactMatch(const Name& name) const {
	return names_.find(name.Join()) != names_.end();
}

int Database::CheckCanonicalForm(const Name& name, std::vector<std::string>& suggestions) const {
	std::string hash;
	NameToHash(name, hash);

	int count = 0;
	std::pair<NamesMap::const_iterator, NamesMap::const_iterator> range =
		canonical_map_.equal_range(hash);

	for (NamesMap::const_iterator i = range.first; i != range.second; ++i, ++count)
		suggestions.push_back(i->second);

	return count;
}

int Database::CheckSpelling(const Name& name, std::vector<std::string>& suggestions, int depth) const {
	std::set<UnicodeString> matches;

	UnicodeString hash;
	NameToUnicodeHash(name, hash);

	spell_trie_.FindApprox(hash, depth, matches);

	int count = 0;
	std::string temp;
	for (std::set<UnicodeString>::const_iterator i = matches.begin(); i != matches.end(); ++i) {
		i->toUTF8String(temp);
		std::pair<NamesMap::const_iterator, NamesMap::const_iterator> range =
			canonical_map_.equal_range(temp);

		for (NamesMap::const_iterator i = range.first; i != range.second; ++i, ++count)
			suggestions.push_back(i->second);
	}

	return count;
}

int Database::CheckStrippedStatus(const Name& name, std::vector<std::string>& matches) const {
	int count = 0;
	std::pair<NamesMap::const_iterator, NamesMap::const_iterator> range =
		stripped_map_.equal_range(name.Join(Name::NORMALIZE_WHITESPACE));

	for (NamesMap::const_iterator i = range.first; i != range.second; ++i, ++count)
		matches.push_back(i->second);

	return count;
}

/*
 * std::string shortcuts to Checks
 */
int Database::CheckExactMatch(const std::string& name) const {
	/* this one a shortcut, actually really */
	return names_.find(name) != names_.end();
}

int Database::CheckCanonicalForm(const std::string& name, std::vector<std::string>& suggestions) const {
	return CheckCanonicalForm(Name(name, locale_), suggestions);
}

int Database::CheckSpelling(const std::string& name, std::vector<std::string>& suggestions, int depth) const {
	return CheckSpelling(Name(name, locale_), suggestions, depth);
}

int Database::CheckStrippedStatus(const std::string& name, std::vector<std::string>& matches) const {
	return CheckStrippedStatus(Name(name, locale_), matches);
}


}
