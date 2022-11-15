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
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../Resources")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../Resources" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
else()
    message(FATAL_ERROR "No resources found!")
endif()

# Setup Externals
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../External)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../External DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

    # Add any sources and define target
    file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../External/imgui/*.cpp
                      ${CMAKE_CURRENT_SOURCE_DIR}/../External/imgui/backends/*glfw.cpp
                      ${CMAKE_CURRENT_SOURCE_DIR}/../External/imgui/backends/*opengl3.cpp
                      ${CMAKE_CURRENT_SOURCE_DIR}/../External/implot/*.cpp
                      ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp)
    add_executable(ImageViewer ${SOURCES})

    # Target the pre-built gl3w .c
	target_sources(ImageViewer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../External/gl3w/src/gl3w.c)

    # Required lib for glfw on Unix
    if (UNIX)
        target_link_libraries(ImageViewer dl)
    endif()

    # Link against glfw
    target_link_libraries(ImageViewer glfw)

	# Setup OpenGL
	find_package(OpenGL REQUIRED)
	target_link_libraries(ImageViewer OpenGL::GL)
    add_compile_definitions(RENDERER_API_OPENGL)

    # Setup nativefiledialog
    target_link_libraries(ImageViewer nfd)
else()
	message(FATAL_ERROR "SetupExternals not called!")
endif()
