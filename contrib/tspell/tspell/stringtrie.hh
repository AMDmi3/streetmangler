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

#ifndef TSPELL_STRINGTRIE_HH
#define TSPELL_STRINGTRIE_HH

#include <string>
#include <set>

#include <tspell/triebase.hh>

namespace TSpell {

template<typename Char>
class StringSetAppender {
public:
	typedef Node<Char> Node;

	typedef std::basic_string<Char> String;
	typedef std::set<String> Set;

private:
	Set& set_;

public:
	StringSetAppender(Set& set) : set_(set) {
	}

	void Append(const Node* node) {
		String str;
		for (const Node* cur = node; cur; cur = cur->parent)
			str += cur->ch;
		std::reverse(str.begin(), str.end());

		set_.insert(str);
	}
};

class StringTrie : public TrieBase<char, StringSetAppender<char> > {
private:
	typedef TrieBase<char, StringSetAppender<char> > Base;

public:
	void Insert(const std::string& string) {
		Base::Insert(string.c_str(), string.length());
	}

	bool FindExact(const std::string& string) const {
		return Base::FindExact(string.c_str(), string.length());
	}

	void FindApprox(const std::string& string, int distance, std::set<std::string>& out) const {
		StringSetAppender<char> a(out);
		Base::FindApprox(string.c_str(), string.length(), distance, a);
	}
};

class WStringTrie : public TrieBase<wchar_t, StringSetAppender<wchar_t> > {
private:
	typedef TrieBase<wchar_t, StringSetAppender<wchar_t> > Base;

public:
	void Insert(const std::wstring& string) {
		Base::Insert(string.c_str(), string.length());
	}

	bool FindExact(const std::wstring& string) const {
		return Base::FindExact(string.c_str(), string.length());
	}

	void FindApprox(const std::wstring& string, int distance, std::set<std::wstring>& out) const {
		StringSetAppender<wchar_t> a(out);
		Base::FindApprox(string.c_str(), string.length(), distance, a);
	}
};

}

#endif
