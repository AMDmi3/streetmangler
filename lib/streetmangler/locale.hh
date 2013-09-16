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

#ifndef STREETMANGLER_LOCALE_HH
#define STREETMANGLER_LOCALE_HH

#include <map>
#include <string>
#include <vector>
#include <exception>

namespace StreetMangler {

/**
 * @todo Make locales persistent via static map
 * @todo Too much string copying - optimize
 */
class Locale {
public:
	enum StatusFlags {
		ORDER_RANDOM_IF_LEFT = 0x01,
		ORDER_RANDOM_IF_RIGHT = 0x02,
		ORDER_RANDOM_IF_SIDE = ORDER_RANDOM_IF_LEFT | ORDER_RANDOM_IF_RIGHT,
		STATUS_AT_LEFT = 0x04,
		STATUS_AT_RIGHT = 0x08,
	};

	struct StatusPartData {
		const char* full;
		const char* canonical;
		const char* abbrev;
		std::vector<const char*> variants;
		int flags;
	};

	typedef std::vector<StatusPartData> StatusPartDataList;

	struct Registrar {
		std::string name_;
		const StatusPartDataList* status_parts_;
		Registrar* next_;

		Registrar(const std::string& name, const StatusPartDataList* status_parts);
	};

	class UnknownLocale : public std::exception {
	private:
		const char* what_;

	public:
		UnknownLocale(const char* what = "") : what_(what) {
		}

		const char* what() const noexcept {
			return what_;
		}
	};

	class BadLocale : public std::exception {
	private:
		const char* what_;

	public:
		BadLocale(const char* what = "") : what_(what) {
		}

		const char* what() const noexcept {
			return what_;
		}
	};

private:
	static Registrar* locales_;

public:
	class StatusPart {
	private:
		int priority_;
		std::string full_;
		std::string canonical_;
		std::string abbrev_;
		int flags_;

	public:
		StatusPart(int priority, const std::string& full, const std::string& canonical, const std::string& abbrev, int flags)
			  : priority_(priority),
			    full_(full),
			    canonical_(canonical),
			    abbrev_(abbrev),
			    flags_(flags) {
		}

		const std::string& GetFull() const { return full_; }
		const std::string& GetCanonical() const { return canonical_; }
		const std::string& GetAbbrev() const { return abbrev_; }
		int GetFlags() const { return flags_; }

		bool IsPrior(const StatusPart* other) const { return other == NULL || priority_ < other->priority_; }
	};

private:
	typedef std::vector<StatusPart> StatusPartVector;
	typedef std::map<std::string, const StatusPart*> StatusPartMap;

	StatusPartVector status_parts_;
	StatusPartMap status_part_by_any_;

public:
	Locale(const std::string& name);

	const StatusPart* FindStatus(const std::string& name) const;
};

}

#endif
