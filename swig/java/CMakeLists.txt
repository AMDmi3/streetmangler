# depends
FIND_PACKAGE(JNI)
INCLUDE_DIRECTORIES(${JNI_INCLUDE_DIRS})

# definitions
SET_SOURCE_FILES_PROPERTIES(../streetmangler.i PROPERTIES CPLUSPLUS ON)
SET(CMAKE_SWIG_OUTDIR org/openstreetmap/StreetMangler)
SET(CMAKE_SWIG_FLAGS -module StreetMangler -package org.openstreetmap.StreetMangler)

# targets
SWIG_ADD_MODULE(streetmangler-java java ../streetmangler.i)
SWIG_LINK_LIBRARIES(streetmangler-java streetmangler)
SET_TARGET_PROPERTIES(streetmangler-java PROPERTIES OUTPUT_NAME StreetMangler)

# tests
ADD_TEST(java_test false)
#ADD_TEST(java_test java -classpath=/usr/lib/junit4.jar:${CMAKE_CURRENT_SOURCE_DIR} TestCase)
