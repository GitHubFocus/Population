#Linux: need to install swig and python-dev
#Ubuntu/Debian: aptitude install swig python-dev

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
message(STATUS "With SWIG, you must compile in release mode")
set(CMAKE_BUILD_TYPE Release)
FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_SWIG_OUTDIR ${POPULATION_LIB_DIR})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_SWIG")
SET(CMAKE_SWIG_FLAGS "-DHAVE_SWIG")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${POPULATION_LIB_DIR})
SET_SOURCE_FILES_PROPERTIES(population.i PROPERTIES CPLUSPLUS ON)
SWIG_ADD_MODULE(population python population.i ${POPULATION_SOURCES})
SWIG_LINK_LIBRARIES(population ${PYTHON_LIBRARIES} ${POPULATION_LIBRARY})
file(COPY ${PROJECT_SOURCE_DIR}/other/tutorial/firststep.py DESTINATION ${POPULATION_LIB_DIR})
