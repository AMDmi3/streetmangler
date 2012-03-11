/*
 * Copyright (C) 2011-2012 Dmitry Marakasov
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

#include <cassert>

#include <streetmangler/locale.hh>

namespace StreetMangler {

Locale::Registrar* Locale::locales_ = NULL;

Locale::Registrar::Registrar(const std::string& name, StatusPartData* status_parts)
	  : name_(name),
	    status_parts_(status_parts),
	    next_(Locale::locales_) {
#ifndef NDEBUG
	/* check for duplicate locale name */
	for (const Registrar* cur = Locale::locales_; cur; cur = cur->next_)
		assert(cur->name_ != name);
#endif

	Locale::locales_ = this;
}

Locale::Locale(const std::string& name) {
	/* first, find locale in linked list */
	const Registrar* locale = NULL;
	for (const Registrar* cur = locales_; cur; cur = cur->next_) {
		if (cur->name_ == name) {
			locale = cur;
			break;
		}
	}

	if (!locale)
		throw UnknownLocale();

	/* process locale data */
	int priority = 1;
	for (StatusPartData* in = locale->status_parts_; in->full; ++in, ++priority) {
		/* (use full status as abbreviation where there's no canonical abbreviation */
		assert(in->full);
		std::string canonical = in->canonical ? in->canonical : in->full;
		std::string abbrev = in->abbrev ? in->abbrev : canonical;
		status_parts_.push_back(StatusPart(priority, in->full, canonical, abbrev, in->flags));
	}

	/* now vector won't change so it's safe to use pointers */
	int i = 0;
	std::pair<StatusPartMap::iterator, bool> res;
	for (StatusPartData* in = locale->status_parts_; in->full; ++in, ++i) {
		for (const char** variant = in->variants; *variant; ++variant) {
			res = status_part_by_any_.insert(std::make_pair(*variant, &status_parts_[i]));
			assert(res.second); /* duplicate keys not allowed */
		}
	}
}

const Locale::StatusPart* Locale::FindStatus(const std::string& name) const {
	StatusPartMap::const_iterator status = status_part_by_any_.find(name);
	if (status == status_part_by_any_.end())
		return NULL;
	return status->second;
}

}
