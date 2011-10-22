PROJECT(streetmangler)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# depends
FIND_PACKAGE(EXPAT REQUIRED)
FIND_PACKAGE(ICU REQUIRED)

FIND_PACKAGE(SWIG)
IF (SWIG_FOUND)
	ADD_SUBDIRECTORY( swig )
ENDIF (SWIG_FOUND)

# common sources
SET(LOCALE_SRCS
	lib/locales/ru.cc
)

SET(LIBRARY_SRCS
	${LOCALE_SRCS}
	lib/database.cc
	lib/locale.cc
	lib/name.cc
)

# definitions
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib ${PROJECT_SOURCE_DIR}/contrib/tspell)
ADD_DEFINITIONS(-DDEFAULT_DATAFILE="${PROJECT_SOURCE_DIR}/data/ru.txt")

ADD_DEFINITIONS(-Wall -Wextra)

# library
INCLUDE_DIRECTORIES(${ICU_INCLUDE_DIR})
ADD_LIBRARY(streetmangler SHARED ${LIBRARY_SRCS})
TARGET_LINK_LIBRARIES(streetmangler ${ICU_LIBRARY})

# utilities
INCLUDE_DIRECTORIES(${EXPAT_INCLUDE_DIRS})
ADD_EXECUTABLE(from_osm utils/from_osm.cc utils/name_extractor.cc)
TARGET_LINK_LIBRARIES(from_osm streetmangler ${EXPAT_LIBRARY})

# tests
ADD_EXECUTABLE(locale_test tests/locale_test.cc)
TARGET_LINK_LIBRARIES(locale_test streetmangler)

ADD_EXECUTABLE(tokenizer_test tests/tokenizer_test.cc)
TARGET_LINK_LIBRARIES(tokenizer_test streetmangler)

ADD_EXECUTABLE(database_test tests/database_test.cc)
TARGET_LINK_LIBRARIES(database_test streetmangler)

ADD_EXECUTABLE(canonical_test tests/canonical_test.cc)
TARGET_LINK_LIBRARIES(canonical_test streetmangler)

ENABLE_TESTING()
ADD_TEST(locale_test locale_test)
ADD_TEST(tokenizer_test tokenizer_test)
ADD_TEST(database_test database_test)
ADD_TEST(canonical_test canonical_test)
