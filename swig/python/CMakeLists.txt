# depends
FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

FIND_PACKAGE(PythonInterp)

# definitions
SET_SOURCE_FILES_PROPERTIES(../streetmangler.i PROPERTIES CPLUSPLUS ON)

# targets
SWIG_ADD_MODULE(streetmangler-python python ../streetmangler.i)
SWIG_LINK_LIBRARIES(streetmangler-python ${PYTHON_LIBRARIES} streetmangler)
SET_TARGET_PROPERTIES(_streetmangler-python PROPERTIES OUTPUT_NAME _streetmangler)

# tests
ADD_TEST(python_test ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/python_test.py)
SET_PROPERTY(TEST python_test PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}" )
