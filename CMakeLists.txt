cmake_minimum_required(VERSION 3.1)

project(Networking_SFML CXX)

set(CMAKE_CXX_STANDARD 14)
if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -lpthread")
endif (UNIX)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)


# function that groups source files together
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)



# Include sub directories
add_subdirectory(Client)
include_directories(Client)

add_subdirectory(Server)
include_directories(Server)

# Server libraries 
set(SERVER_EXECUTABLE_NAME "Server")
assign_source_group(${SERVER_SOURCES})
add_executable(${SERVER_EXECUTABLE_NAME} ${SERVER_SOURCES})

if (UNIX)
	find_package(Threads REQUIRED)
	if (THREADS_FOUND)
		target_link_libraries(${SERVER_EXECUTABLE_NAME} -lpthread -lm)
	endif (THREADS_FOUND)
endif (UNIX)

find_package(SFML 2 REQUIRED system network)
if (SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${SERVER_EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif (SFML_FOUND)


set(CLIENT_EXECUTABLE_NAME "Client")
add_executable(${CLIENT_EXECUTABLE_NAME} ${CLIENT_SOURCES})
assign_source_group(${CLIENT_SOURCES})


find_package(SFML 2 REQUIRED system window graphics network)
if (SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${CLIENT_EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif (SFML_FOUND)


set_target_properties(${SERVER_EXECUTABLE_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(${CLIENT_EXECUTABLE_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)