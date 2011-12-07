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
#include <map>
#include <string>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <unicode/unistr.h>

#include <tspell/unitrie.hh>

#include <streetmangler/database.hh>
#include <streetmangler/locale.hh>
#include <streetmangler/name.hh>

namespace {
	static const UnicodeString g_yo = UnicodeString::fromUTF8("ё");
	static const UnicodeString g_ye = UnicodeString::fromUTF8("е");
};

namespace StreetMangler {

class Database::Private {
	friend class Database;
private:
	Private(const Locale& locale) : locale_(locale) {
	}

	const Locale& GetLocale() const {
		return locale_;
	}

	void NameToHashes(const Name& name, std::string* plainhash, UnicodeString* uhash, UnicodeString* uhashordered, int extraflags = 0) const {
		static const int flags = Name::STATUS_TO_LEFT | Name::EXPAND_STATUS | Name::NORMALIZE_WHITESPACE | Name::NORMALIZE_PUNCT;
		/* base for a hash - lowercase name with status part at left */
		UnicodeString base_hash = UnicodeString::fromUTF8(name.Join(flags | extraflags)).toLower();

		if (uhash)
			*uhash = base_hash;

		if (plainhash)
			base_hash.toUTF8String(*plainhash);

		if (uhashordered) {
			std::vector<UnicodeString> words;

			int32_t start = 0;
			int32_t end;
			while ((end = base_hash.indexOf(' ', start)) != -1) {
				if (start != end)
					words.push_back(UnicodeString(base_hash, start, end-start));
				start = end + 1;
			}
			if (start != base_hash.length())
				words.push_back(UnicodeString(base_hash, start));

			/* sort all words except for the status part */
			bool sortall = (extraflags & Name::REMOVE_ALL_STATUSES) || !name.HasStatusPart();
			std::sort(sortall ? words.begin() : ++words.begin(), words.end());

			*uhashordered = UnicodeString();
			for (std::vector<UnicodeString>::iterator i = words.begin(); i != words.end(); ++i) {
				if (i != words.begin())
					*uhashordered += " ";
				*uhashordered += *i;
			}
		}
	}

private:
    typedef std::set<std::string> NamesSet;
    typedef std::multimap<std::string, std::string> NamesMap;
    typedef std::multimap<UnicodeString, std::string> UnicodeNamesMap;

private:
    const Locale& locale_;
    NamesSet names_;
    NamesMap canonical_map_;
    UnicodeNamesMap spelling_map_;
    UnicodeNamesMap stripped_map_;

    TSpell::UnicodeTrie spell_trie_;
};

Database::Database(const Locale& locale) : private_(new Database::Private(locale)) {
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
	return private_->locale_;
}

void Database::Add(const std::string& name) {
	Name tokenized(name, private_->locale_);

	std::string hash;
	UnicodeString uhash;
	UnicodeString uhashordered;

	private_->NameToHashes(tokenized, &hash, &uhash, &uhashordered);

	/* for the locales in which canonical form != full form,
	 * we need to use canonical form as a reference */
	std::string canonical = tokenized.Join(Name::CANONICALIZE_STATUS);

	/* for exact match */
	private_->names_.insert(canonical);

	/* for canonical form  */
	private_->canonical_map_.insert(std::make_pair(hash, canonical));

	/* for spelling */
	private_->spell_trie_.Insert(uhash);
	private_->spelling_map_.insert(std::make_pair(uhash, canonical));
	if (uhash != uhashordered) {
		private_->spell_trie_.Insert(uhashordered);
		private_->spelling_map_.insert(std::make_pair(uhashordered, canonical));
	}

	/* for stripped status  */
	UnicodeString stripped_uhashordered;
	private_->NameToHashes(tokenized, NULL, NULL, &stripped_uhashordered, Name::REMOVE_ALL_STATUSES);
	stripped_uhashordered.findAndReplace(g_yo, g_ye);
	if (stripped_uhashordered != uhashordered)
		private_->stripped_map_.insert(std::make_pair(stripped_uhashordered, canonical));
}

/*
 * Checks
 */
int Database::CheckExactMatch(const Name& name) const {
	return private_->names_.find(name.Join()) != private_->names_.end();
}

int Database::CheckCanonicalForm(const Name& name, std::vector<std::string>& suggestions) const {
	std::string hash;
	private_->NameToHashes(name, &hash, NULL, NULL);

	int count = 0;
	std::pair<Private::NamesMap::const_iterator, Private::NamesMap::const_iterator> range =
		private_->canonical_map_.equal_range(hash);

	for (Private::NamesMap::const_iterator i = range.first; i != range.second; ++i, ++count)
		suggestions.push_back(i->second);

	return count;
}

int Database::CheckSpelling(const Name& name, std::vector<std::string>& suggestions, int depth) const {
	UnicodeString hash, hashordered;
	private_->NameToHashes(name, NULL, &hash, &hashordered);

	std::set<UnicodeString> matches;
	for (int i = 0; matches.empty() && i <= depth; ++i) {
		private_->spell_trie_.FindApprox(hash, i, matches);
		private_->spell_trie_.FindApprox(hashordered, i, matches);
	}

	int count = 0;
	for (std::set<UnicodeString>::const_iterator i = matches.begin(); i != matches.end(); ++i) {
		std::pair<Private::UnicodeNamesMap::const_iterator, Private::UnicodeNamesMap::const_iterator> range =
			private_->spelling_map_.equal_range(*i);

		for (Private::UnicodeNamesMap::const_iterator i = range.first; i != range.second; ++i, ++count)
			suggestions.push_back(i->second);
	}

	return count;
}

int Database::CheckStrippedStatus(const Name& name, std::vector<std::string>& matches) const {
	UnicodeString uhashordered;
	private_->NameToHashes(name, NULL, NULL, &uhashordered);
	uhashordered.findAndReplace(g_yo, g_ye);

	int count = 0;
	std::pair<Private::UnicodeNamesMap::const_iterator, Private::UnicodeNamesMap::const_iterator> range =
		private_->stripped_map_.equal_range(uhashordered);

	for (Private::UnicodeNamesMap::const_iterator i = range.first; i != range.second; ++i, ++count)
		matches.push_back(i->second);

	return count;
}

/*
 * std::string shortcuts to Checks
 */
int Database::CheckExactMatch(const std::string& name) const {
	/* this one a shortcut, actually really */
	return private_->names_.find(name) != private_->names_.end();
}

int Database::CheckCanonicalForm(const std::string& name, std::vector<std::string>& suggestions) const {
	return CheckCanonicalForm(Name(name, private_->locale_), suggestions);
}

int Database::CheckSpelling(const std::string& name, std::vector<std::string>& suggestions, int depth) const {
	return CheckSpelling(Name(name, private_->locale_), suggestions, depth);
}

int Database::CheckStrippedStatus(const std::string& name, std::vector<std::string>& matches) const {
	return CheckStrippedStatus(Name(name, private_->locale_), matches);
}

}
