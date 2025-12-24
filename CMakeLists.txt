cmake_minimum_required(VERSION 3.15)
project(MyOpenGLProject VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(ENABLE_WARNINGS "Enable compiler warnings" ON)

# ============================================
# Fetch Dependencies
# ============================================
include(FetchContent)

# GLFW
message(STATUS "Fetching GLFW...")
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLM
message(STATUS "Fetching GLM...")
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

# ============================================
# GLAD Configuration (Manual)
# ============================================
set(GLAD_DIR ${CMAKE_SOURCE_DIR}/external/glad)
if(NOT EXISTS ${GLAD_DIR}/src/glad.c)
    message(FATAL_ERROR 
        "GLAD not found! Please download GLAD:\n"
        "1. Visit https://glad.dav1d.de/\n"
        "2. Configure: Language=C/C++, API=OpenGL 3.3+, Profile=Core\n"
        "3. Click 'Generate' and download the ZIP file\n"
        "4. Extract to: ${GLAD_DIR}/\n"
        "   Required structure:\n"
        "   - ${GLAD_DIR}/include/glad/glad.h\n"
        "   - ${GLAD_DIR}/include/KHR/khrplatform.h\n"
        "   - ${GLAD_DIR}/src/glad.c"
    )
endif()

add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

# ============================================
# STB Configuration (Manual)
# ============================================
set(STB_DIR ${CMAKE_SOURCE_DIR}/external/stb)
if(NOT EXISTS ${STB_DIR}/stb_image.h)
    message(WARNING 
        "STB not found at ${STB_DIR}/stb_image.h\n"
        "Download it from: https://raw.githubusercontent.com/nothings/stb/master/stb_image.h\n"
        "Place it in: ${STB_DIR}/stb_image.h"
    )
else()
    message(STATUS "STB found at ${STB_DIR}")
endif()

# Create interface library for STB
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${STB_DIR})

# ============================================
# Source Files
# ============================================
# Start with main.cpp (required)
set(SOURCE_FILES
    src/main.cpp
     src/app/app.cpp
     src/config.cpp
    src/view/shader.cpp
    src/systems/camera_system.cpp
    src/systems/motion_system.cpp
    src/systems/render_system.cpp
)

# Add optional source files if they exist
set(OPTIONAL_SOURCES
    src/config.cpp
    src/triangle_mesh.cpp
    src/material.cpp
    src/linear_algebros.cpp
   
   # src/factory/factory.cpp
)

foreach(src ${OPTIONAL_SOURCES})
    if(EXISTS ${CMAKE_SOURCE_DIR}/${src})
        list(APPEND SOURCE_FILES ${src})
    endif()
endforeach()

# Header files (optional for IDE)
set(HEADERS)
set(OPTIONAL_HEADERS
    include/config.h
    include/triangle_mesh.h
    include/material.h
    include/linear_algebros.h
    include/app/app.h
    include/component/camera_component.h
    include/component/physics_component.h
    include/component/render_component.h
    include/component/transform_component.h
    include/view/shader.h
    include/systems/camera_system.h
    include/systems/render_system.h
    include/systems/motion_system.h
   # include/factory/factory.h
)

foreach(hdr ${OPTIONAL_HEADERS})
    if(EXISTS ${CMAKE_SOURCE_DIR}/${hdr})
        list(APPEND HEADERS ${hdr})
    endif()
endforeach()

# Shader files (optional for IDE)
set(SHADERS)
set(OPTIONAL_SHADERS
    src/shaders/vertex.txt
    src/shaders/fragment.txt
)

foreach(shdr ${OPTIONAL_SHADERS})
    if(EXISTS ${CMAKE_SOURCE_DIR}/${shdr})
        list(APPEND SHADERS ${shdr})
    endif()
endforeach()

# Verify critical source files exist
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/src/main.cpp)
    message(FATAL_ERROR "src/main.cpp not found! This is required.")
endif()

# Combine all sources
set(ALL_SOURCES ${SOURCE_FILES} ${HEADERS} ${SHADERS})

# Group files for IDEs
if(HEADERS)
    source_group("Header Files" FILES ${HEADERS})
endif()
if(SHADERS)
    source_group("Shaders" FILES ${SHADERS})
endif()
source_group("Source Files" FILES ${SOURCE_FILES})

# ============================================
# Executable
# ============================================
add_executable(${PROJECT_NAME} ${ALL_SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glad
        glfw
        glm::glm
        stb
)

# Compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE
)

# Compiler warnings
if(ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX-)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE 
            -Wall -Wextra -Wpedantic
            $<$<CONFIG:Debug>:-g>
            $<$<CONFIG:Release>:-O3>
        )
    endif()
endif()

# ============================================
# Platform-Specific Settings
# ============================================
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
    # Set console subsystem for int main()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
elseif(UNIX AND NOT APPLE)
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        OpenGL::GL 
        ${CMAKE_DL_LIBS}
        pthread
    )
elseif(APPLE)
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
endif()

# ============================================
# MSVC-Specific Settings
# ============================================
if(MSVC)
    # Set startup project
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
        PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME}
    )
    
    # Enable multi-processor compilation
    target_compile_options(${PROJECT_NAME} PRIVATE /MP)
    
    # Set working directory for debugging
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()

# ============================================
# Post-Build: Copy Resources
# ============================================
# Copy shader files to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    COMMENT "Copying shader files to build directory"
)

# Copy image files to build directory
if(EXISTS ${CMAKE_SOURCE_DIR}/img)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/img
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/img
        COMMENT "Copying image files to build directory"
    )
endif()

# Copy any additional resources (textures, models, etc.)
if(EXISTS ${CMAKE_SOURCE_DIR}/resources)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
        COMMENT "Copying resources to build directory"
    )
endif()

# ============================================
# Installation (Optional)
# ============================================
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/shaders
    DESTINATION bin
)

if(EXISTS ${CMAKE_SOURCE_DIR}/img)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/img
        DESTINATION bin
    )
endif()

# ============================================
# Summary
# ============================================
message(STATUS "")
message(STATUS "=== ${PROJECT_NAME} Configuration ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Output Directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "====================================")
message(STATUS "")