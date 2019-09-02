project( PCMesh3 )

if (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Release")
endif()

if (CMAKE_BUILD_TYPE MATCHES "Debug")
	add_definitions(-DDEBUG)
endif()

SET(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)

cmake_minimum_required(VERSION 2.8.11)

# CGAL and its components
find_package( CGAL QUIET COMPONENTS  )

if ( NOT CGAL_FOUND )

  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()

endif()


# include helper file
include( ${CGAL_USE_FILE} )


# Boost and its components
find_package( Boost REQUIRED )

if ( NOT Boost_FOUND )

  message(STATUS "This project requires the Boost library, and will not be compiled.")

  return()

endif()

# include for local directory
include_directories( BEFORE include )

# include for local package


# Creating entries for all C++ files with "main" routine
# ##########################################################

include( CGAL_CreateSingleSourceCGALProgram )

create_single_source_cgal_program( "main.cpp" )


