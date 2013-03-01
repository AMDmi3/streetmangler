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

#include <stdexcept>
#include <sstream>

#include <expat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "osm_name_extractor.hh"

OsmNameExtractor::OsmNameExtractor() {
}

OsmNameExtractor::~OsmNameExtractor() {
}

void OsmNameExtractor::AddAddrTag(const std::string& tag) {
	addr_tags_.insert(tag);
}

void OsmNameExtractor::AddNameTag(const std::string& tag) {
	name_tags_.insert(tag);
}

void OsmNameExtractor::ParseFile(const char* filename) {
	int f;
	if ((f = open(filename, O_RDONLY)) == -1)
		throw std::runtime_error(std::string("Cannot open OSM file: ") + strerror(errno));

	try {
		Parse(f);
	} catch (...) {
		close(f);
		throw;
	}

	close(f);
}

void OsmNameExtractor::ParseStdin() {
	return Parse(0);
}

void OsmNameExtractor::Parse(int fd) {
	XML_Parser parser = NULL;

	if ((parser = XML_ParserCreate(NULL)) == NULL)
		throw std::runtime_error("cannot create XML parser");

	XML_SetElementHandler(parser, StartElement, EndElement);
	XML_SetUserData(parser, this);

	/* Parse file */
	try {
		char buf[65536];
		ssize_t len;
		do {
			if ((len = read(fd, buf, sizeof(buf))) < 0)
				throw std::runtime_error(std::string("read error: ") + strerror(errno));
			if (XML_Parse(parser, buf, len, len == 0) == XML_STATUS_ERROR)
				throw std::runtime_error(std::string("expat error: ") + XML_ErrorString(XML_GetErrorCode(parser)));
		} while (len != 0);
	} catch (std::runtime_error &e) {
		std::stringstream ss;
		ss << "error parsing input: " << e.what() << " at line " << XML_GetCurrentLineNumber(parser) << " pos " << XML_GetCurrentColumnNumber(parser);
		XML_ParserFree(parser);
		throw std::runtime_error(ss.str());
	} catch (...) {
		XML_ParserFree(parser);
		throw;
	}

	XML_ParserFree(parser);
}

void OsmNameExtractor::StartElement(void* userData, const char* name, const char** atts) {
	OsmNameExtractor* parser = static_cast<OsmNameExtractor*>(userData);

	if (strcmp(name, "node") == 0 || strcmp(name, "way") == 0) {
		parser->addrs_.clear();
		parser->names_.clear();
		parser->highway_.clear();
	}
	if (strcmp(name, "tag") != 0)
		return;

	std::string k, v;
	for (const char** att = atts; *att; att += 2) {
		if (att[0][0] == 'k')
			k = att[1];
		else if (att[0][0] == 'v')
			v = att[1];
	}

	/* save relevant tags which will be processed in EndElement */
	if (k == "highway") {
		if (v != "bus_stop" && v != "emergency_access_point")
			parser->highway_ = v;
	} else if (parser->addr_tags_.find(k) != parser->addr_tags_.end() && !v.empty()) {
		parser->addrs_.push_back(v);
	} else if (parser->name_tags_.find(k) != parser->name_tags_.end() && !v.empty()) {
		parser->names_.push_back(v);
	}
}

void OsmNameExtractor::EndElement(void* userData, const char* name) {
	OsmNameExtractor* parser = static_cast<OsmNameExtractor*>(userData);

	enum { NODE, WAY, OTHER } type = OTHER;

	if (strcmp(name, "node") == 0)
		type = NODE;
	else if (strcmp(name, "way") == 0)
		type = WAY;
	else
		return;

	// Process addr tags for all objects
	for (std::vector<std::string>::const_iterator i = parser->addrs_.begin(); i != parser->addrs_.end(); ++i)
		parser->ProcessName(*i);

	// Process names for highways
	if (type == WAY && !parser->highway_.empty()) {
		for (std::vector<std::string>::const_iterator i = parser->names_.begin(); i != parser->names_.end(); ++i)
			parser->ProcessName(*i);
	}
}
