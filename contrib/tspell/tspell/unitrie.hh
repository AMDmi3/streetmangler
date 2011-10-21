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
	typedef Node<UChar> Node;

	typedef std::set<UnicodeString> Set;

private:
	Set& set_;

public:
	UnicodeStringSetAppender(Set& set) : set_(set) {
	}

	void Append(const Node* node) {
		UnicodeString str;
		for (const Node* cur = node; cur; cur = cur->parent)
			str += cur->ch;
		str.reverse();

		set_.insert(str);
	}
};

class UnicodeTrie : public TrieBase<UChar, UnicodeStringSetAppender> {
private:
	typedef TrieBase<UChar, UnicodeStringSetAppender> Base;

public:
	void Insert(const UnicodeString& string) {
		Base::Insert(string.getBuffer(), string.length());
	}

	bool FindExact(const UnicodeString& string) const {
		return Base::FindExact(string.getBuffer(), string.length());
	}

	void FindApprox(const UnicodeString& string, int distance, std::set<UnicodeString>& out) const {
		UnicodeStringSetAppender a(out);
		Base::FindApprox(string.getBuffer(), string.length(), distance, a);
	}
};

}

#endif
