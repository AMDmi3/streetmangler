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

#ifndef STREETMANGLER_NAME_HH
#define STREETMANGLER_NAME_HH

#include <streetmangler/locale.hh>

namespace StreetMangler {

class Name {
public:
	enum JoinFlags {
		STATUS_TO_LEFT       = 0x01,
		STATUS_TO_RIGHT      = 0x02,
		EXPAND_STATUS        = 0x04,
		SHRINK_STATUS        = 0x08,
		NORMALIZE_WHITESPACE = 0x10,
		REMOVE_ALL_STATUSES  = 0x20,
		NORMALIZE_PUNCT      = 0x40,
	};

private:
	enum TokenType {
		SPACE,
		PUNCT,
		ALPHA,
		NONE,
	};

	struct Token {
		TokenType type;
		std::string text;
		const Locale::StatusPart* status_part;

		Token() : status_part(NULL) {
		}

		Token(TokenType ty, std::string tx) : type(ty), text(tx), status_part(NULL) {
		}

		bool IsValid() const {
			return !text.empty();
		}
	};

private:
	typedef std::vector<Token> TokenVector;

private:
	const Locale& locale_;
	TokenVector tokens_;
	int status_pos_;

public:
	Name(const std::string& name, const Locale& locale);

	std::string Join(int flags = 0) const;

	operator std::string() const { return Join(); }
};

}

#endif
