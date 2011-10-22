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
#include <iostream>
#include <map>
#include <set>

#include <err.h>

#include <streetmangler/database.hh>
#include <streetmangler/locale.hh>
#include <streetmangler/name.hh>
#include "name_extractor.hh"

#ifndef DEFAULT_DATAFILE
#	define DEFAULT_DATAFILE "streets.txt"
#endif

#ifndef DEFAULT_LOCALE
#	define DEFAULT_LOCALE "ru_RU"
#endif

class NameAggregator {
public:
	NameAggregator(StreetMangler::Database& db, bool perstreet_stats, int spelldistance) :
		database_(db),
		count_all_(0),
		count_exact_match_(0),
		count_canonical_form_(0),
		count_spelling_fixed_(0),
		count_stripped_status_(0),
		count_no_match_(0),
		perstreet_stats_(perstreet_stats),
		spelldistance_(spelldistance) {
	}

	void ProcessName(const std::string& name) {
		++count_all_;

		if (perstreet_stats_) {
			all_.insert(name);
		} else if (!all_.insert(name).second) {
			/* if we don't need perstreet statistics,
			 * we may skip processing of already processed
			 * streets */
			return;
		}

		std::vector<std::string> suggestions;

		if (database_.CheckExactMatch(name)) {
			++count_exact_match_;
			exact_match_.insert(name);
		} else if (database_.CheckCanonicalForm(name, suggestions)) {
			++count_canonical_form_;
			canonical_form_.insert(std::make_pair(name, suggestions.front()));
			suggestions.clear();
		} else if (spelldistance_ > 0 && database_.CheckSpelling(name, suggestions, spelldistance_)) {
			++count_spelling_fixed_;
			spelling_fixed_.insert(std::make_pair(name, suggestions.front()));
			suggestions.clear();
		} else if (database_.CheckStrippedStatus(name, suggestions)) {
			++count_stripped_status_;
			stripped_status_.insert(name);
			suggestions.clear();
		} else {
			++count_no_match_;
			no_match_.insert(name);
		}
	}

	void DumpStats() {
		fprintf(stderr, "           Total       Exact match     Canonical form     Spelling fixed    Stripped status           No match\n");
		/*                Total: 00000000 00000000 ( 00.00%) 00000000 ( 00.00%) 00000000 ( 00.00%) 00000000 ( 00.00%) 00000000 ( 00.00%)*/
		if (perstreet_stats_) {
			fprintf(stderr, " Total: %8d %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%)\n",
				count_all_,
				count_exact_match_, (float)count_exact_match_/(float)count_all_*100.0f,
				count_canonical_form_, (float)count_canonical_form_/(float)count_all_*100.0f,
				count_spelling_fixed_, (float)count_spelling_fixed_/(float)count_all_*100.0f,
				count_stripped_status_, (float)count_stripped_status_/(float)count_all_*100.0f,
				count_no_match_, (float)count_no_match_/(float)count_all_*100.0f);
		}
		fprintf(stderr, "Unique: %8d %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%) %8d (%6.2f%%)\n",
			(int)all_.size(),
			(int)exact_match_.size(), (float)exact_match_.size()/(float)all_.size()*100.0f,
			(int)canonical_form_.size(), (float)canonical_form_.size()/(float)all_.size()*100.0f,
			(int)spelling_fixed_.size(), (float)spelling_fixed_.size()/(float)all_.size()*100.0f,
			(int)stripped_status_.size(), (float)stripped_status_.size()/(float)all_.size()*100.0f,
			(int)no_match_.size(), (float)no_match_.size()/(float)all_.size()*100.0f);
	}

	void DumpData() {
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
		for (SuggestionMap::const_iterator i = spelling_fixed_.begin(); i != spelling_fixed_.end(); ++i)
			dump << i->first << "|" << i->second << std::endl;
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
	}

private:
	typedef std::set<std::string> NameSet;
	typedef std::map<std::string, std::string> SuggestionMap;

private:
	StreetMangler::Database& database_;

	int count_all_;
	int count_exact_match_;
	int count_canonical_form_;
	int count_spelling_fixed_;
	int count_stripped_status_;
	int count_no_match_;

	bool perstreet_stats_;

	int spelldistance_;

	NameSet all_;
	NameSet exact_match_;
	SuggestionMap canonical_form_;
	SuggestionMap spelling_fixed_;
	NameSet stripped_status_;
	NameSet no_match_;
};

class OsmNameExtractor : public NameExtractor {
private:
	NameAggregator& aggregator_;

public:
	OsmNameExtractor(NameAggregator& aggregator) : aggregator_(aggregator) {
	}

	virtual void ProcessName(const std::string& name) {
		aggregator_.ProcessName(name);
	}
};

int usage(const char* progname, int code) {
	fprintf(stderr, "Usage: %s [-dhs] [-l locale] [-f database] file.osm\n", progname);
	fprintf(stderr, "  -s  display per-street statistics (takes extra time)\n");
	fprintf(stderr, "  -d  dump street lists into dump.*\n");
	fprintf(stderr, "  -p  spelling check distance (default 1)\n");
	fprintf(stderr, "  -h  display this help\n");
	fprintf(stderr, "  -l  set locale (default \""DEFAULT_LOCALE"\")\n");
	fprintf(stderr, "  -f  specify pats to street names database (default "DEFAULT_DATAFILE")\n");
	fprintf(stderr, "      (may be specified more than once)\n");
	return code;
}

int main(int argc, char** argv) {
	const char* progname = argv[0];
	const char* localename = DEFAULT_LOCALE;
	bool dumpflag = false;
	bool statsflag = false;
	int spelldistance = 1;

	std::vector<const char*> datafiles;

	/* process options */
	int c;
    while ((c = getopt(argc, argv, "sdhf:l:p:")) != -1) {
		switch (c) {
			case 's': statsflag = true; break;
			case 'd': dumpflag = true; break;
			case 'f': datafiles.push_back(optarg); break;
			case 'l': localename = optarg; break;
			case 'p': spelldistance = (int)strtoul(optarg, 0, 10); break;
			case 'h': return usage(progname, 0);
			default:
				return usage(progname, 1);
		}
	}

	/* if no databases were specified, use the default one */
	if (datafiles.empty())
		datafiles.push_back(DEFAULT_DATAFILE);

	argc -= optind;
	argv += optind;

	/* there should be some input */
	if (argc < 1)
		return usage(progname, 1);

	/* setup and load the database */
	StreetMangler::Locale locale(localename);
	StreetMangler::Database database(locale);

	for (std::vector<const char*>::const_iterator i = datafiles.begin(); i != datafiles.end(); ++i) {
		fprintf(stderr, "Loading database \"%s\"...\n", *i);
		database.Load(*i);
	}

	/* process all input files */
	NameAggregator aggregator(database, statsflag, spelldistance);

	for (int i = 0; i < argc; ++i) {
		std::string file(argv[i]);
		if (file.rfind(".osm") == file.length() - 4) {
			fprintf(stderr, "Processing file \"%s\" as OSM data...\n", file.c_str());
			OsmNameExtractor(aggregator).ParseFile(file.c_str());
//		} else if (file.rfind(".txt") == file.length() - 4) {
//			fprintf(stderr, "Processing file \"%s\" as strings list...\n", file.c_str());
		} else {
			errx(1, "%s: unknown format (we only support .osm and .txt)\n", file.c_str());
		}
	}

	/* produce aggregated dump and statistics */
	if (dumpflag) {
		fprintf(stderr, "Dumping data...\n");
		aggregator.DumpData();
	}

	aggregator.DumpStats();

	return 0;
}
