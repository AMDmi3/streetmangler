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

#ifndef NAME_AGGREGATOR_HH
#define NAME_AGGREGATOR_HH

#include <map>
#include <set>

#include <streetmangler/database.hh>
#include <streetmangler/name.hh>

class NameAggregator {
public:
	enum Flags {
		PERSTREET_STATS = 0x01,
		COUNT_NAMES = 0x02,
	};

public:
	NameAggregator(StreetMangler::Database& db, int flags, int spelldistance);

	void ProcessName(const std::string& name);
	void DumpStats();
	void DumpData();

private:
	typedef std::set<std::string> NameSet;
	typedef std::map<std::string, std::string> SuggestionMap;
	typedef std::map<std::string, std::vector<std::string> > MultiSuggestionMap;

	typedef std::map<std::string, int> NameCountMap;

private:
	StreetMangler::Database& database_;

	int count_all_;
	int count_exact_match_;
	int count_canonical_form_;
	int count_spelling_fixed_;
	int count_stripped_status_;
	int count_no_match_;
	int count_non_name_;

	int flags_;

	int spelldistance_;

	NameSet all_;
	NameSet exact_match_;
	MultiSuggestionMap canonical_form_;
	MultiSuggestionMap spelling_fixed_;
	NameSet stripped_status_;
	NameSet no_match_;
	NameSet non_name_;

	NameCountMap counts_all_;
	NameCountMap counts_no_match_;
	NameCountMap counts_spelling_fixed_;
	NameCountMap counts_stripped_status_;
	NameCountMap counts_non_name_;
};

#endif
