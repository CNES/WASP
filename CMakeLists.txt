project(NewTemporalSynthesis)

cmake_minimum_required(VERSION 2.8)

include(CTest)

find_package(OTB REQUIRED)
find_program(BASH_Program bash)

include(${OTB_USE_FILE})
include(FindPythonInterp)

# Set a default build type if none was specified
set(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wno-missing-field-initializers")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

find_package(Boost REQUIRED COMPONENTS unit_test_framework filesystem)
add_definitions(-DBOOST_TEST_DYN_LINK)

message("Project binary folder:" ${PROJECT_BINARY_DIR})
add_definitions(-DVCL_CAN_STATIC_CONST_INIT_FLOAT=0)

add_subdirectory(Common)
add_subdirectory(MuscateMetadata)
add_subdirectory(CompositePreprocessing)
add_subdirectory(WeightCalculation)
add_subdirectory(UpdateSynthesis)
add_subdirectory(ProductFormatter)
add_subdirectory(PythonScripts)

install(FILES ./Scripts/WASP PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_EXECUTE DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" ON)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

message("Installing WASP to: " ${CMAKE_INSTALL_PREFIX})
