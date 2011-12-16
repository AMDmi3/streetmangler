# depends
FIND_PACKAGE(PerlLibs)
INCLUDE_DIRECTORIES(${PERL_INCLUDE_PATH})

FIND_PACKAGE(Perl)

# definitions
SET_SOURCE_FILES_PROPERTIES(../streetmangler.i PROPERTIES CPLUSPLUS ON)
SET(CMAKE_SWIG_FLAGS -module StreetMangler)

# targets
SWIG_ADD_MODULE(streetmangler-perl perl ../streetmangler.i)
SWIG_LINK_LIBRARIES(streetmangler-perl ${PERL_LIBRARIES} streetmangler)
SET_TARGET_PROPERTIES(streetmangler-perl PROPERTIES OUTPUT_NAME StreetMangler)

# tests
ADD_TEST(perl_test ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/perl_test.pl)
SET_PROPERTY(TEST perl_test PROPERTY ENVIRONMENT "PERL5LIB=${CMAKE_CURRENT_BINARY_DIR}" )
