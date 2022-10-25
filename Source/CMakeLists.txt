# Define target
add_executable(ImageViewer "Main.cpp")

# OpenMP for Eigen
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

    add_compile_definitions(HAS_OMP)
endif()

# Setup threading
if (UNIX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif (UNIX)

# Copy any resources
if (EXISTS "../Resources")
    file(COPY "../Resources" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

# Setup required .c files from gl3w
if (EXISTS "../External/gl3w/src/gl3w.c")
    target_sources(ImageViewer PUBLIC "../External/gl3w/src/gl3w.c")

    if (UNIX)
        target_link_libraries(ImageViewer dl)
    endif()
else()
    message(FATAL_ERROR "gl3w not setup!")
endif()
