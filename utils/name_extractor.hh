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

#ifndef NAME_EXTRACTOR_HH
#define NAME_EXTRACTOR_HH

#include <string>

/**
 * OSM XML parser which extracts street names from street objects and addresses
 */
class NameExtractor {
public:
	NameExtractor();
	virtual ~NameExtractor();

	/** Parse a file */
	void ParseFile(const char* filename);

	/** Parse standart input */
	void ParseStdin();

protected:
	/** Street name handler
	 *
	 * Implement it in derived class to handle street names */
	virtual void ProcessName(const std::string& name) = 0;

private:
	void Parse(int fd);
	static void StartElement(void* userData, const char* name, const char** atts);
	static void EndElement(void* userData, const char* name);

private:
	std::string name_;
	std::string highway_;
	std::string addr_street_;
};

#endif
