PROJECT(streetmangler)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

ENABLE_TESTING()

# options
OPTION(WITH_PYTHON "Enable python bindings" ON)
OPTION(WITH_PERL "Enable perl bindings" ON)
OPTION(WITH_CPLUSPLUS11 "Enable C++11" ON)

# depends
FIND_PACKAGE(EXPAT REQUIRED)
FIND_PACKAGE(ICU REQUIRED)

# common sources
SET(LOCALE_SRCS
	lib/locales/ru.cc
	lib/locales/en.cc
	lib/locales/uk.cc
)

SET(LIBRARY_SRCS
	${LOCALE_SRCS}
	lib/database.cc
	lib/locale.cc
	lib/name.cc
	lib/stringlistparser.cc
)

SET(PROCESS_NAMES_SRCS
	utils/process_names.cc
	utils/osm_name_extractor.cc
	utils/name_aggregator.cc
)

# definitions
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib ${PROJECT_SOURCE_DIR}/contrib/tspell)
ADD_DEFINITIONS(-DDATADIR="${PROJECT_SOURCE_DIR}/data")

ADD_DEFINITIONS(-Wall -Wextra -pedantic)

IF(WITH_CPLUSPLUS11)
	ADD_DEFINITIONS(-std=c++11 -DWITH_CPLUSPLUS11)
ENDIF(WITH_CPLUSPLUS11)

# library
INCLUDE_DIRECTORIES(${ICU_INCLUDE_DIR})
ADD_LIBRARY(streetmangler SHARED ${LIBRARY_SRCS})
TARGET_LINK_LIBRARIES(streetmangler ${ICU_LIBRARY})

# bindings
IF(WITH_PYTHON OR WITH_PERL)
	ADD_SUBDIRECTORY(swig)
ENDIF(WITH_PYTHON OR WITH_PERL)

# utilities
INCLUDE_DIRECTORIES(${EXPAT_INCLUDE_DIRS})
ADD_EXECUTABLE(process_names ${PROCESS_NAMES_SRCS})
TARGET_LINK_LIBRARIES(process_names streetmangler ${EXPAT_LIBRARY})

# tests
ADD_EXECUTABLE(locale_test tests/locale_test.cc)
TARGET_LINK_LIBRARIES(locale_test streetmangler)

ADD_EXECUTABLE(tokenizer_test tests/tokenizer_test.cc)
TARGET_LINK_LIBRARIES(tokenizer_test streetmangler)

ADD_EXECUTABLE(database_test tests/database_test.cc)
TARGET_LINK_LIBRARIES(database_test streetmangler)

ADD_EXECUTABLE(canonical_test tests/canonical_test.cc)
TARGET_LINK_LIBRARIES(canonical_test streetmangler)

ADD_TEST(locale_test locale_test)
ADD_TEST(tokenizer_test tokenizer_test)
ADD_TEST(database_test database_test)
ADD_TEST(canonical_test canonical_test)
