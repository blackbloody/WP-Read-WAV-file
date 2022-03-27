cmake_minimum_required(VERSION 3.11.4)
#cmake_minimum_required(VERSION 2.8)

project(WP_WAV)

SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

FIND_PACKAGE(ALSA)
IF(ALSA_FOUND)
	SET(WP_WAV_HAVE_ALSA TRUE)
ELSE(ALSA_FOUND)
	MESSAGE("ALSA does not exist..need to install it libasound2-dev")
ENDIF(ALSA_FOUND)

FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(GTKMM gtkmm-3.0)

include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})

FILE (GLOB_RECURSE SOURCE_LIBS src/app/*.cpp)

FILE (GLOB_RECURSE SOURCE_LIBS_READER_WAV src/reader_wav/*.cpp)
add_library(LIBS_READER_WAV SHARED ${SOURCE_LIBS_READER_WAV})
target_include_directories(LIBS_READER_WAV PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/reader_wav )

FILE (GLOB_RECURSE SOURCE_LIBS_SIGNAL_GL src/app/signal_reader/*.cpp)
add_library(LIBS_SIGNAL_GL SHARED ${SOURCE_LIBS_SIGNAL_GL})
target_include_directories(LIBS_SIGNAL_GL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/app/signal_reader )

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp ${SOURCE_LIBS})

target_link_libraries(${CMAKE_PROJECT_NAME} ${CMAKE_DL_LIBS})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/app/app_header )
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/app/models )

target_link_libraries(${CMAKE_PROJECT_NAME} LIBS_READER_WAV )
target_link_libraries(${CMAKE_PROJECT_NAME} LIBS_SIGNAL_GL )
target_link_libraries(${CMAKE_PROJECT_NAME} ${GTKMM_LIBRARIES})

find_package(SFML 2.5.1 COMPONENTS graphics REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} sfml-graphics )

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${PYTHON_LIBRARIES})

find_package(pybind11 REQUIRED) 
target_link_libraries(${CMAKE_PROJECT_NAME} pybind11::embed)

IF(WP_WAV_HAVE_ALSA)
	target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${ALSA_INCLUDE_DIRS})
	target_link_libraries(${CMAKE_PROJECT_NAME} ${ALSA_LIBRARIES})
ENDIF(WP_WAV_HAVE_ALSA)


find_package(OpenGL)
include_directories( ${OPENGL_INCLUDE_DIRS} )
target_link_libraries(${CMAKE_PROJECT_NAME} ${OPENGL_LIBRARIES} )
