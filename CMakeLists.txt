PROJECT(streetmangler)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

ENABLE_TESTING()

# options
OPTION(WITH_PYTHON "Enable python bindings" OFF)
OPTION(WITH_PERL "Enable perl bindings" OFF)

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

SET(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
SET(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
SET(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} --coverage")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11")

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
SET(TESTS locale_test locale_internal_test tokenizer_test database_test canonical_test)
FOREACH(TEST ${TESTS})
	ADD_EXECUTABLE(${TEST} tests/${TEST}.cc)
	TARGET_LINK_LIBRARIES(${TEST} streetmangler)
	ADD_TEST(${TEST} ${TEST})
ENDFOREACH(TEST ${TESTS})
