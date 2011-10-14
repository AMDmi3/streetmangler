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

#include <list>

#include <unicode/unistr.h>

#include <streetmangler/name.hh>

namespace StreetMangler {

Name::Name(const std::string& name, const Locale& locale) : locale_(locale), status_pos_(-1) {
	/* split name into tokens */
	Token curtoken;
	for (const char* cur = name.c_str(); *cur; ++cur) {
		TokenType curtype;
		switch (*cur) {
		case ' ': case '\t': curtype = SPACE; break;
		case '.': case ',': curtype = PUNCT; break;
		default: curtype = ALPHA; break;
		}

		if (curtype != curtoken.type) {
			if (curtoken.IsValid())
				tokens_.push_back(curtoken);
			curtoken = Token();
			curtoken.type = curtype;
		}
		curtoken.text += *cur;
	}

	if (curtoken.IsValid())
		tokens_.push_back(curtoken);

	/* find status part */
	const Locale::StatusPart* bestpart = NULL;
	for (unsigned int i = 0; i < tokens_.size(); ++i) {
		std::string lowercase;
		UnicodeString::fromUTF8(tokens_[i].text).toLower().toUTF8String(lowercase);

		const Locale::StatusPart* part;
		if ((part = locale_.FindStatus(lowercase)) != NULL) {
			tokens_[i].status_part = part;

			if (part->IsPrior(bestpart)) {
				bestpart = part;
				status_pos_ = i;
			}
		}
	}
}

std::string Name::Join(int flags) const {
	typedef std::list<Token> OutList;
	OutList outtokens;

	/* create temporary token list */
	OutList::iterator status = outtokens.end();
	int n = 0;
	for (TokenVector::const_iterator i = tokens_.begin(); i != tokens_.end(); ++i, ++n) {
		outtokens.push_back(*i);
		if (n == status_pos_)
			status = --outtokens.end();
	}

	/* following code works with statuses, do nothing where none is found */
	if (status != outtokens.end()) {
		/* unconditionally normalize punctuation if status part is moved */
		if (flags & (STATUS_TO_LEFT | STATUS_TO_RIGHT | REMOVE_ALL_STATUSES))
			flags |= NORMALIZE_PUNCT;

		OutList::iterator tmp;
		/* change status part inplace */
		if (flags & (STATUS_MODE_MASK | NORMALIZE_PUNCT)) {
			/* modify status */
			switch (flags & STATUS_MODE_MASK) {
			case EXPAND_STATUS:       status->text = status->status_part->GetFull(); break;
			case SHRINK_STATUS:       status->text = status->status_part->GetAbbrev(); break;
			case CANONICALIZE_STATUS: status->text = status->status_part->GetCanonical(); break;
			}

			/* if there was a dot, merge it into status */
			OutList::iterator dot = status;
			if (++dot != outtokens.end() && dot->text == ".") {
				/* XXX: this quirk is rather ugly */
				if (!(flags & (EXPAND_STATUS | SHRINK_STATUS)))
					status->text += ".";
				if (++(tmp = dot) != outtokens.end() && tmp->type != SPACE) {
					dot->type = SPACE;
					dot->text = " ";
				} else
					outtokens.erase(dot);
			}

			if (flags & NORMALIZE_WHITESPACE) {
				OutList::iterator comma = status;
				if (comma != outtokens.begin() && (--comma)->text == ",")
					outtokens.insert(status, Token(SPACE, " "));
			}

			if (flags & NORMALIZE_PUNCT) {
				OutList::iterator comma = status;
				/* processes "Ленина,улица" */
				if (comma != outtokens.begin() && (--comma)->text == ",") {
					comma->type = SPACE;
					comma->text = " ";
				}
				/* processes "Ленина, улица" */
				if (comma->type == SPACE && comma != outtokens.begin() && (--comma)->text == ",") {
					outtokens.erase(comma);
				}
			}
		}

		bool atleft = status == outtokens.begin();
		bool atright = status == --outtokens.end();

		if (flags & REMOVE_ALL_STATUSES) {
			/* remove everything what looks lile status part */
			for (OutList::iterator cur = outtokens.begin(); cur != outtokens.end(); ) {
				if (!cur->status_part) {
					++cur;
					continue;
				}

				int sides = 0;
				if (cur != outtokens.begin() && (--(tmp = cur))->type == SPACE) {
					outtokens.erase(tmp);
					++sides;
				}
				if (cur != --outtokens.end() && (++(tmp = cur))->type == SPACE) {
					outtokens.erase(tmp);
					++sides;
				}

				if (sides == 2) {
					cur->type = SPACE;
					cur->text = " ";
					++cur;
				} else {
					OutList::iterator tmp = cur;
					++cur;
					outtokens.erase(tmp);
				}
			}
		} else if ((!atleft && (flags & STATUS_TO_LEFT)) || (!atright && (flags & STATUS_TO_RIGHT))) {
			/* remove status part from its place */
			Token saved_status = *status;

			int sides = 0;
			if (status != outtokens.begin() && (--(tmp = status))->type == SPACE) {
				outtokens.erase(tmp);
				++sides;
			}
			if (status != --outtokens.end() && (++(tmp = status))->type == SPACE) {
				outtokens.erase(tmp);
				++sides;
			}

			if (sides == 2) {
				status->type = SPACE;
				status->text = " ";
			} else {
				outtokens.erase(status);
			}

			/* and paste it at front/back */
			if (flags & STATUS_TO_LEFT) {
				if (!outtokens.empty())
					outtokens.push_front(Token(SPACE, " "));
				outtokens.push_front(saved_status);
			} else if (flags & STATUS_TO_RIGHT) {
				if (!outtokens.empty())
					outtokens.push_back(Token(SPACE, " "));
				outtokens.push_back(saved_status);
			}
		}
	}

	/* join string together performing final whitespace normalization if needed */
	std::string out;
	if (flags & NORMALIZE_WHITESPACE) {
		OutList::const_iterator i = outtokens.begin();
		for (; i != outtokens.end() && i->type == SPACE; ++i) {}

		bool wasspace = false;
		for (; i != outtokens.end(); ++i) {
			if (i->type == SPACE && wasspace) {
				/* do nothing */
			} else if (i->type == SPACE) {
				out += " ";
				wasspace = true;
			} else {
				out += i->text;
				wasspace = false;
			}
		}

		int newsize = out.size();
		while (newsize > 0 && (out[newsize-1] == ' ' || out[newsize-1] == '\t'))
			newsize--;
		out.resize(newsize);
	} else {
		for (OutList::const_iterator i = outtokens.begin(); i != outtokens.end(); ++i)
			out += i->text;
	}

	return out;
}

}
