# depends
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

# flags
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w") # disable warnings for generated code

# bindings
IF(WITH_PYTHON)
	ADD_SUBDIRECTORY(python)
ENDIF(WITH_PYTHON)

IF(WITH_PERL)
	ADD_SUBDIRECTORY(perl)
ENDIF(WITH_PERL)
