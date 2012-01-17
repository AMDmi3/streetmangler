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

#include <fstream>
#include <iomanip>

#include "name_aggregator.hh"

NameAggregator::NameAggregator(StreetMangler::Database& db, int flags, int spelldistance) :
	database_(db),
	count_all_(0),
	count_exact_match_(0),
	count_canonical_form_(0),
	count_spelling_fixed_(0),
	count_stripped_status_(0),
	count_no_match_(0),
	count_non_name_(0),
	flags_(flags),
	spelldistance_(spelldistance) {
}

void NameAggregator::ProcessName(const std::string& name) {
	++count_all_;

	if (flags_ & COUNT_NAMES)
		++counts_all_[name];

	if (flags_ & (PERSTREET_STATS | COUNT_NAMES)) {
		all_.insert(name);
	} else if (!all_.insert(name).second) {
		/* if we don't need perstreet statistics or name counts,
		 * we may skip processing of already processed streets */
		return;
	}

	/* exact match */
	if (database_.CheckExactMatch(name)) {
		++count_exact_match_;
		exact_match_.insert(name);
		return;
	}

	/* miscellaneous types of mismatch */
	std::vector<std::string> suggestions;
	StreetMangler::Name tokenized(name, database_.GetLocale());

	if (database_.CheckCanonicalForm(tokenized, suggestions)) {
		++count_canonical_form_;
		canonical_form_.insert(std::make_pair(name, suggestions.front()));
		return;
	}

	if (database_.CheckSpelling(tokenized, suggestions, spelldistance_)) {
		++count_spelling_fixed_;
		std::pair<MultiSuggestionMap::iterator, bool> insresult =
			spelling_fixed_.insert(std::make_pair(name, std::vector<std::string>()));
		if (insresult.second) /* false possible in -s mode */
			suggestions.swap(insresult.first->second);

		if (flags_ & COUNT_NAMES)
			++counts_spelling_fixed_[name];

		return;
	}

	if (database_.CheckStrippedStatus(tokenized, suggestions)) {
		++count_stripped_status_;
		stripped_status_.insert(name);

		if (flags_ & COUNT_NAMES)
			++counts_stripped_status_[name];

		return;
	}

	if (tokenized.HasStatusPart()) {
		++count_no_match_;
		no_match_.insert(name);

		if (flags_ & COUNT_NAMES)
			++counts_no_match_[name];

		return;
	}

	++count_non_name_;
	non_name_.insert(name);

	if (flags_ & COUNT_NAMES)
		++counts_non_name_[name];
}

void NameAggregator::DumpStats() {
	fprintf(stderr, "Classification statistics:\n");
	fprintf(stderr, "           Total       Exact match     Canonical form     Spelling fixed    Stripped status           No match          Non-names\n");
	/*                Total: 00000000 00000000 ( 00.00%) 00000000 ( 00.00%) 00000000 ( 00.00%) 00000000 ( 00.00%) 00000000 ( 00.00%) 00000000 ( 00.00%)*/
	if (flags_ & PERSTREET_STATS) {
		float total = count_all_ > 0 ? count_all_ : 1.0f;
		fprintf(stderr, " Total: %8d %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%)\n",
			count_all_,
			count_exact_match_, (float)count_exact_match_/total*100.0f,
			count_canonical_form_, (float)count_canonical_form_/total*100.0f,
			count_spelling_fixed_, (float)count_spelling_fixed_/total*100.0f,
			count_stripped_status_, (float)count_stripped_status_/total*100.0f,
			count_no_match_, (float)count_no_match_/total*100.0f,
			count_non_name_, (float)count_non_name_/total*100.0f);
	}
	{
		float total = all_.size() > 0 ? all_.size() : 1.0f;
		fprintf(stderr, "Unique: %8d %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%)\n",
			(int)all_.size(),
			(int)exact_match_.size(), (float)exact_match_.size()/total*100.0f,
			(int)canonical_form_.size(), (float)canonical_form_.size()/total*100.0f,
			(int)spelling_fixed_.size(), (float)spelling_fixed_.size()/total*100.0f,
			(int)stripped_status_.size(), (float)stripped_status_.size()/total*100.0f,
			(int)no_match_.size(), (float)no_match_.size()/total*100.0f,
			(int)non_name_.size(), (float)non_name_.size()/total*100.0f);
	}

	fprintf(stderr, "Generalized database statistics:\n");
	fprintf(stderr, "           Total             Match            Fixable           No match\n");
	/*                Total: 00000000 00000000 ( 00.00%) 00000000 ( 00.00%) 00000000 ( 00.00%)*/
	if (flags_ & PERSTREET_STATS) {
		int streets = count_all_ - count_stripped_status_ - count_non_name_;
		float total = streets > 0 ? streets : 1.0f;

		int fixable = count_canonical_form_ + count_spelling_fixed_;

		fprintf(stderr, " Total: %8d %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%)\n",
				streets,
				count_exact_match_, (float)count_exact_match_/total*100.0f,
				fixable, (float)fixable/total*100.0f,
				count_no_match_, (float)count_no_match_/total*100.0f);
	}
	{
		int streets = all_.size() - stripped_status_.size() - non_name_.size();
		float total = streets > 0 ? streets : 1.0f;

		int fixable = canonical_form_.size() + spelling_fixed_.size();

		fprintf(stderr, "Unique: %8d %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%)\n",
				streets,
				(int)exact_match_.size(), (float)exact_match_.size()/total*100.0f,
				fixable, (float)fixable/total*100.0f,
				(int)no_match_.size(), (float)no_match_.size()/total*100.0f);
	}
}

void NameAggregator::DumpData() {
	std::ofstream dump;

	dump.open("dump.all.txt");
	for (NameSet::const_iterator i = all_.begin(); i != all_.end(); ++i)
		dump << *i << std::endl;
	dump.close();

	dump.open("dump.exact_match.txt");
	for (NameSet::const_iterator i = exact_match_.begin(); i != exact_match_.end(); ++i)
		dump << *i << std::endl;
	dump.close();

	dump.open("dump.canonical_form.txt");
	for (SuggestionMap::const_iterator i = canonical_form_.begin(); i != canonical_form_.end(); ++i)
		dump << i->first << "|" << i->second << std::endl;
	dump.close();

	dump.open("dump.spelling_fixed.txt");
	for (MultiSuggestionMap::const_iterator i = spelling_fixed_.begin(); i != spelling_fixed_.end(); ++i) {
		dump << i->first;
		if (i->second.size() == 1) {
			dump << "|" << i->second.front() << std::endl;
		} else {
			dump << " # (";
			for (std::vector<std::string>::const_iterator s = i->second.begin(); s != i->second.end(); ++s) {
				if (s != i->second.begin())
					dump << ", ";
				dump << *s;
			}
			dump << ")" << std::endl;
		}
	}
	dump.close();

	dump.open("dump.stripped_status.txt");
	for (NameSet::const_iterator i = stripped_status_.begin(); i != stripped_status_.end(); ++i)
		dump << *i << std::endl;
	dump.close();

	dump.open("dump.no_match.txt");
	for (NameSet::const_iterator i = no_match_.begin(); i != no_match_.end(); ++i)
		dump << *i << std::endl;
	dump.close();

	dump.open("dump.no_match.full.txt");
	for (NameSet::const_iterator i = no_match_.begin(); i != no_match_.end(); ++i)
		dump << StreetMangler::Name(*i, database_.GetLocale()).Join(StreetMangler::Name::EXPAND_STATUS) << std::endl;
	dump.close();

	dump.open("dump.non_name.txt");
	for (NameSet::const_iterator i = non_name_.begin(); i != non_name_.end(); ++i)
		dump << *i << std::endl;
	dump.close();

	if (flags_ & COUNT_NAMES) {
		dump.open("dump.counts.all.txt");
		for (NameCountMap::const_iterator i = counts_all_.begin(); i != counts_all_.end(); ++i)
			dump << std::setw(6) << i->second << " " << i->first << std::endl;
		dump.close();

		dump.open("dump.counts.spelling_fixed.txt");
		for (NameCountMap::const_iterator i = counts_spelling_fixed_.begin(); i != counts_spelling_fixed_.end(); ++i)
			dump << std::setw(6) << i->second << " " << i->first << std::endl;
		dump.close();

		dump.open("dump.counts.stripped_status.txt");
		for (NameCountMap::const_iterator i = counts_stripped_status_.begin(); i != counts_stripped_status_.end(); ++i)
			dump << std::setw(6) << i->second << " " << i->first << std::endl;
		dump.close();

		dump.open("dump.counts.no_match.txt");
		for (NameCountMap::const_iterator i = counts_no_match_.begin(); i != counts_no_match_.end(); ++i)
			dump << std::setw(6) << i->second << " " << i->first << std::endl;
		dump.close();

		dump.open("dump.counts.non_name.txt");
		for (NameCountMap::const_iterator i = counts_non_name_.begin(); i != counts_non_name_.end(); ++i)
			dump << std::setw(6) << i->second << " " << i->first << std::endl;
		dump.close();
	}
}
