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

# Setup Externals
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../External)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../External DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

	target_sources(ImageViewer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../External/gl3w/src/gl3w.c)

    if (UNIX)
        target_link_libraries(ImageViewer dl)
    endif()

    target_link_libraries(ImageViewer glfw)

	# Setup OpenGL
	find_package(OpenGL REQUIRED)
	target_link_libraries(ImageViewer OpenGL::GL)

	# Setup ImGui
	file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../External/imgui/*.cpp)
    file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../External/imgui/backends/*.cpp)
    file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../External/implot/*.cpp)
else()
	message(FATAL_ERROR "SetupExternals not called!")
endif()
