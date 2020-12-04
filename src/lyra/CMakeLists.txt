
file(GLOB_RECURSE LYRA_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/common_lyra/*.cpp)
file(GLOB_RECURSE LYRA_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/common_lyra/*.hpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

option(BUILD_DOC "Build documentation" ON)

if(BUILD_DOC)
    message("-- Build documentation ON")
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
#        message("Doxygen build started")

        add_custom_target(DocDoxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
else(BUILD_DOC)
    message("-- Build documentation OFF")
endif(BUILD_DOC)


add_executable(MultiParts ${LYRA_SOURCE_FILES} ${LYRA_HEADER_FILES} multiparts/main.cpp)
target_link_libraries(MultiParts ${MPI_LIBRARIES})
set_target_properties(MultiParts PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS} ${CMAKE_CXX_FLAGS}")
set_target_properties(MultiParts PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")

add_executable(Solve ${LYRA_SOURCE_FILES} ${LYRA_HEADER_FILES} solve/main.cpp)
target_link_libraries(Solve ${MPI_LIBRARIES})
set_target_properties(Solve PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS} ${CMAKE_CXX_FLAGS}")
set_target_properties(Solve PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")