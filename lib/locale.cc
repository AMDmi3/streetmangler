/*
 * Copyright (C) 2011-2013 Dmitry Marakasov
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


#include <streetmangler/locale.hh>

namespace StreetMangler {

Locale::Registrar* Locale::locales_ = nullptr;

Locale::Registrar::Registrar(const std::string& name, const StatusPartDataList* status_parts)
	  : name_(name),
	    status_parts_(status_parts),
	    next_(Locale::locales_) {
#ifndef NDEBUG
	/* check for duplicate locale name */
	for (const Registrar* cur = Locale::locales_; cur; cur = cur->next_)
		if (cur->name_ == name)
			throw BadLocale("duplicate locale name (attempt to register multiple locales with a same name)");
#endif

	Locale::locales_ = this;
}

Locale::Locale(const std::string& name) {
	/* first, find locale in linked list */
	const Registrar* locale = nullptr;
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
	for (StatusPartDataList::const_iterator in = locale->status_parts_->cbegin(); in != locale->status_parts_->cend(); ++in, ++priority) {
		/* (use full status as abbreviation where there's no canonical abbreviation */
		if (in->full == nullptr)
			throw BadLocale("full form of a status part not specified");
		std::string canonical = in->canonical ? in->canonical : in->full;
		std::string abbrev = in->abbrev ? in->abbrev : canonical;
		status_parts_.push_back(StatusPart(priority, in->full, canonical, abbrev, in->flags));
	}

	/* now vector won't change so it's safe to use pointers */
	int i = 0;
	std::pair<StatusPartMap::iterator, bool> res;
	for (StatusPartDataList::const_iterator in = locale->status_parts_->cbegin(); in != locale->status_parts_->cend(); ++in, ++i) {
		for (std::vector<const char*>::const_iterator variant = in->variants.cbegin(); variant != in->variants.cend(); ++variant) {
			res = status_part_by_any_.insert(std::make_pair(*variant, &status_parts_[i]));
			if (!res.second)
				throw BadLocale("duplicate status part variants not allowed");
		}
	}
}

const Locale::StatusPart* Locale::FindStatus(const std::string& name) const {
	StatusPartMap::const_iterator status = status_part_by_any_.find(name);
	if (status == status_part_by_any_.end())
		return nullptr;
	return status->second;
}

}
