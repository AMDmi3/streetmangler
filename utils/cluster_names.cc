/*
 * Copyright (C) 2014 Dmitry Marakasov
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

#include <unordered_set>
#include <iostream>

#include <streetmangler/stringlistparser.hh>
#include <unicode/unistr.h>
#include <tspell/unitrie.hh>

class NameProcessor : public StreetMangler::StringListParser {
private:
	std::set<UnicodeString> names_;

public:
	NameProcessor(const std::string& filename) : StreetMangler::StringListParser(filename) {
	}

	void DumpGraphviz() {
		TSpell::UnicodeTrie trie;

		for (auto& name : names_)
			trie.Insert(name);

		std::cout << "graph streets {" << std::endl;

		for (auto& name : names_) {
			std::set<UnicodeString> similars;
			trie.FindApprox(name, 1, similars);

			for (auto& similar : similars) {
				if (name < similar) {
					std::string utfname, utfsimilar;
					name.toUTF8String(utfname);
					similar.toUTF8String(utfsimilar);
					std::cout << "\t\"" << utfname << "\" -- \"" << utfsimilar << "\"" << std::endl;
				}
			}
		}

		std::cout << "}" << std::endl;
	}

protected:
	virtual void ProcessString(const std::string& string) {
		names_.insert(UnicodeString::fromUTF8(string));
	}
};

int usage(const char* progname, int exitcode) {
	std::cerr << "Usage: " << progname << " <file>" << std::endl;

	exit(exitcode);
}

int main(int argc, char** argv) {
	if (argc != 2)
		usage(argv[0], 1);

	NameProcessor processor(argv[1]);
	processor.Parse();
	processor.DumpGraphviz();

	return 0;
}
