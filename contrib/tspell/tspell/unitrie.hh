/*
 * Copyright (C) 2011 Dmitry Marakasov
 *
 * This file is part of tspell.
 *
 * tspell is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tspell is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tspell.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TSPELL_UNITRIE_HH
#define TSPELL_UNITRIE_HH

#include <set>

#include <unicode/unistr.h>

#include <tspell/triebase.hh>

namespace TSpell {

class UnicodeStringSetAppender {
public:
	typedef Node<UChar> node_type;

	typedef std::set<icu::UnicodeString> set_type;

private:
	set_type& set_;

public:
	UnicodeStringSetAppender(set_type& set) : set_(set) {
	}

	void Append(const node_type* node) {
		icu::UnicodeString str;
		for (const node_type* cur = node; cur; cur = cur->parent)
			str += cur->ch;
		str.reverse();

		set_.insert(str);
	}
};

class UnicodeTrie : public TrieBase<UChar, UnicodeStringSetAppender> {
private:
	typedef TrieBase<UChar, UnicodeStringSetAppender> base_type;

public:
	void Insert(const icu::UnicodeString& string) {
		base_type::Insert(string.getBuffer(), string.length());
	}

	bool FindExact(const icu::UnicodeString& string) const {
		return base_type::FindExact(string.getBuffer(), string.length());
	}

	void FindApprox(const icu::UnicodeString& string, int distance, std::set<icu::UnicodeString>& out) const {
		UnicodeStringSetAppender a(out);
		base_type::FindApprox(string.getBuffer(), string.length(), distance, a);
	}
};

}

#endif
