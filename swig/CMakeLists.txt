# depends
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

FIND_PACKAGE(PythonInterp)

# definitions
ADD_DEFINITIONS(-Wno-unused-parameter -Wno-self-assign -Wno-missing-field-initializers)

# targets
SET_SOURCE_FILES_PROPERTIES(streetmangler.i PROPERTIES CPLUSPLUS ON)
SWIG_ADD_MODULE(streetmangler python streetmangler.i)
SWIG_LINK_LIBRARIES(streetmangler ${PYTHON_LIBRARIES} streetmangler)

# tests
ADD_TEST(python_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/python_test.py)
SET_PROPERTY(TEST python_test PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}" )
