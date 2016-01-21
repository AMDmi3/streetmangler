/*
 * Copyright (C) 2011-2016 Dmitry Marakasov
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
#include <unordered_set>
#include <unordered_map>

#include <string>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <unicode/unistr.h>
#include <unicode/uchar.h>

#include <tspell/unitrie.hh>

#include <streetmangler/database.hh>
#include <streetmangler/locale.hh>
#include <streetmangler/name.hh>
#include <streetmangler/stringlistparser.hh>

namespace {
	static const std::string g_include_command = ".include";

	// XXX: unhardcode, move to locale
	static const UnicodeString g_yo = UnicodeString::fromUTF8("ё");
	static const UnicodeString g_ye = UnicodeString::fromUTF8("е");

	int PickDist(int olddist, int newdist) {
		return (olddist < 0 || (newdist >= 0 && newdist < olddist)) ? newdist : olddist;
	}

	class DatabaseLoader : public StreetMangler::StringListParser {
	private:
		StreetMangler::Database& database_;

	public:
		DatabaseLoader(const std::string& filename, StreetMangler::Database& database) : StreetMangler::StringListParser(filename), database_(database) {
		}

		void ProcessString(const std::string& string) {
			if (string.find(g_include_command) == 0) {
				/* process include directive */
				size_t filepos = g_include_command.length();
				while (filepos < string.length() && (string[filepos] == ' ' || string[filepos] == '\t'))
					++filepos;

				size_t slashpos = filename_.rfind("/");

				std::string newname;
				if (slashpos == std::string::npos) {
					newname = string.substr(filepos);
				} else {
					newname = filename_.substr(0, slashpos + 1);
					newname += string.substr(filepos);
				}

				DatabaseLoader recursive_loader(newname, database_);
				recursive_loader.Parse();
			} else {
				/* process name */
				database_.Add(string);
			}
		}
	};
}

namespace StreetMangler {

class Database::Private {
	friend class Database;
protected:
	Private(const Locale& locale) : locale_(locale) {
	}

	const Locale& GetLocale() const {
		return locale_;
	}

	void NameToHashes(const Name& name, std::string* plainhash, UnicodeString* uhash, UnicodeString* uhashordered, UnicodeString* uhashunordered, int extraflags = 0) const {
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

		if (uhashunordered)
			*uhashunordered = UnicodeString::fromUTF8(name.Join((flags & ~Name::STATUS_TO_LEFT) | extraflags)).toLower();
	}

	static int GetRealApproxDistance(const UnicodeString& sample, const UnicodeString& match, int realdepth) {
		/* exact match */
		if (realdepth == 0)
			return 0;

		const UnicodeString& shorter = sample.length() < match.length() ? sample : match;
		const UnicodeString& longer = sample.length() < match.length() ? match : sample;

		int left, right;

		/* get common prefix and suffix */
		for (left = 0; left < shorter.length() && shorter[left] == longer[left]; ++left) {
			/* empty */
		}

		for (right = 0; right < shorter.length() - left && shorter[shorter.length() - 1 - right] == longer[longer.length() - 1 - right]; ++right) {
			/* empty */
		}

		UnicodeString shorterdiff = shorter.tempSubString(left, shorter.length() - left - right);
		UnicodeString longerdiff = longer.tempSubString(left, longer.length() - left - right);

		/* check if there're any non-numeric character in the diff */
		bool shorterdiff_numeric = false;
		for (int i = 0; i < shorterdiff.length(); ++i) {
			if (u_isdigit(shorterdiff.char32At(i))) {
				shorterdiff_numeric = true;
				break;
			}
		}

		bool longerdiff_numeric = false;
		for (int i = 0; i < longerdiff.length(); ++i) {
			if (u_isdigit(longerdiff.char32At(i))) {
				longerdiff_numeric = true;
				break;
			}
		}

		if (shorterdiff_numeric || longerdiff_numeric) {
			bool touches_number = false;
			if (left > 0 && u_isdigit(shorter.charAt(left - 1)))
				touches_number = true;
			if (right > 0 && u_isdigit(shorter.charAt(shorter.length() - right)))
				touches_number = true;

			/* numeric-only match */
			if ((shorterdiff_numeric && longerdiff_numeric) || touches_number)
				return -1;
		}

		/* count е->ё as zero depth (for equal length, shorter = match, longer = source) */
		if (realdepth == 1 && shorterdiff.length() == 1 && longerdiff.length() == 1 && shorterdiff[0] == g_yo[0] && longerdiff[0] == g_ye[0])
			return 0;

		/* count swapped adjacent letters as a single typo */
		if (realdepth == 2 && shorterdiff.length() == 2 && longerdiff.length() == 2 && shorterdiff[0] == longerdiff[1] && shorterdiff[1] == longerdiff[0])
			return 1;

		return realdepth;
	}

protected:
	typedef std::unordered_set<std::string> NamesSet;
	typedef std::unordered_multimap<std::string, std::string> NamesMap;
	typedef std::multimap<UnicodeString, std::string> UnicodeNamesMap; // XXX: no hasher fn for UnicodeString

protected:
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

const StreetMangler::Locale& StreetMangler::Database::GetLocale() const {
	return private_->locale_;
}

void Database::Load(const std::string& filename) {
	DatabaseLoader loader(filename, *this);
	loader.Parse();
}

void Database::Add(const std::string& name) {
	Name tokenized(name, private_->locale_);

	std::string hash;
	UnicodeString uhash;
	UnicodeString uhashordered;
	UnicodeString uhashunordered;

	private_->NameToHashes(tokenized, &hash, &uhash, &uhashordered, &uhashunordered);

	/* for the locales in which canonical form != full form,
	 * we need to use canonical form as a reference
	 *
	 * XXX: this has a side effect of ignoring special writing
	 * of status part from the database (e.g. "Русская Слобода" is
	 * converted to "Русская слобода"). May handle canonical form ==
	 * full form case specially just using variant from the database */
	std::set<std::string> canonical_part_variants;

	/* default canonical variant */
	if (tokenized.GetStatusFlags() & Locale::STATUS_AT_LEFT)
		canonical_part_variants.insert(tokenized.Join(Name::CANONICALIZE_STATUS|Name::STATUS_TO_LEFT));
	else if (tokenized.GetStatusFlags() & Locale::STATUS_AT_RIGHT)
		canonical_part_variants.insert(tokenized.Join(Name::CANONICALIZE_STATUS|Name::STATUS_TO_RIGHT));
	else
		canonical_part_variants.insert(tokenized.Join(Name::CANONICALIZE_STATUS));

	/* additional canonical variants, which may be enabled depending on flags */
	if (tokenized.IsStatusPartAtLeft() && (tokenized.GetStatusFlags() & Locale::ORDER_RANDOM_IF_LEFT))
		canonical_part_variants.insert(tokenized.Join(Name::CANONICALIZE_STATUS|Name::STATUS_TO_RIGHT));

	if (tokenized.IsStatusPartAtRight() && (tokenized.GetStatusFlags() & Locale::ORDER_RANDOM_IF_RIGHT))
		canonical_part_variants.insert(tokenized.Join(Name::CANONICALIZE_STATUS|Name::STATUS_TO_LEFT));

	/* for each canonical form, fill structures required to link other forms to it */
	for (std::set<std::string>::iterator canonical = canonical_part_variants.begin(); canonical != canonical_part_variants.end(); ++canonical) {
		/* for exact match */
		private_->names_.insert(*canonical);

		/* for canonical form */
		private_->canonical_map_.insert(std::make_pair(hash, *canonical));

		/* for spelling */
		private_->spell_trie_.Insert(uhashordered);
		private_->spelling_map_.insert(std::make_pair(uhashordered, *canonical));
		if (uhashunordered != uhashordered) {
			private_->spell_trie_.Insert(uhashunordered);
			private_->spelling_map_.insert(std::make_pair(uhashunordered, *canonical));
		}

		/* for stripped status */
		UnicodeString stripped_uhashordered;
		private_->NameToHashes(tokenized, nullptr, nullptr, &stripped_uhashordered, nullptr, Name::REMOVE_ALL_STATUSES);
		stripped_uhashordered.findAndReplace(g_yo, g_ye);
		if (stripped_uhashordered != uhashordered)
			private_->stripped_map_.insert(std::make_pair(stripped_uhashordered, *canonical));
	}
}

/*
 * Checks
 */
int Database::CheckExactMatch(const Name& name) const {
	return private_->names_.find(name.Join()) != private_->names_.end();
}

int Database::CheckCanonicalForm(const Name& name, std::vector<std::string>& suggestions) const {
	std::string hash;
	private_->NameToHashes(name, &hash, nullptr, nullptr, nullptr);

	int count = 0;
	std::pair<Private::NamesMap::const_iterator, Private::NamesMap::const_iterator> range =
		private_->canonical_map_.equal_range(hash);

	for (Private::NamesMap::const_iterator i = range.first; i != range.second; ++i, ++count)
		suggestions.push_back(i->second);

	return count;
}

int Database::CheckSpelling(const Name& name, std::vector<std::string>& suggestions, int depth) const {
	UnicodeString hashordered, hashunordered;
	private_->NameToHashes(name, nullptr, nullptr, &hashordered, &hashunordered);

	int realdepth = 0;
	std::set<UnicodeString> matches;
	for (int i = 0; matches.empty() && i <= depth + 1; ++i) {
		private_->spell_trie_.FindApprox(hashordered, i, matches);
		private_->spell_trie_.FindApprox(hashunordered, i, matches);
		realdepth = i;
	}

	std::set<std::string> suggestions_unique;
	for (std::set<UnicodeString>::const_iterator i = matches.begin(); i != matches.end(); ++i) {
		/* skip matches that differ only in numeric parts */
		int dist = -1;
		dist = PickDist(dist, private_->GetRealApproxDistance(hashordered, *i, realdepth));
		dist = PickDist(dist, private_->GetRealApproxDistance(hashunordered, *i, realdepth));

		if (dist < 0 || dist > depth)
			continue;

		std::pair<Private::UnicodeNamesMap::const_iterator, Private::UnicodeNamesMap::const_iterator> range =
			private_->spelling_map_.equal_range(*i);

		for (Private::UnicodeNamesMap::const_iterator j = range.first; j != range.second; ++j)
			suggestions_unique.insert(j->second);
	}

	suggestions.reserve(suggestions.size() + suggestions_unique.size());

	for (std::set<std::string>::const_iterator i = suggestions_unique.begin(); i != suggestions_unique.end(); ++i)
		suggestions.push_back(*i);

	return suggestions_unique.size();
}

int Database::CheckStrippedStatus(const Name& name, std::vector<std::string>& matches) const {
	UnicodeString uhashordered;
	private_->NameToHashes(name, nullptr, nullptr, &uhashordered, nullptr);
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
