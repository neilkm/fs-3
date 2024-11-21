#
# Mbed OS Project
#
cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.19)

# Initialize Mbed OS build system.
# Note: This block must be before the project() call.
set(MBED_APP_JSON_PATH mbed_app.json5)
# set(CUSTOM_TARGETS_JSON_PATH custom_targets.json) # Uncomment if you need a custom target

# Adjust the path to mbed-os (one directory above)
set(MBED_OS_DIR "${CMAKE_SOURCE_DIR}/../mbed-os")

# Include Mbed OS CMake scripts
include(${MBED_OS_DIR}/tools/cmake/app.cmake)

# Configure upload method for ST-LINK (if applicable)
set(UPLOAD_METHOD_DEFAULT STLINK)
set(STLINK_UPLOAD_ENABLED TRUE)
set(STLINK_LOAD_ADDRESS 0x8000000)
set(STLINK_ARGS --connect-under-reset)

# Add mbed-os as a subdirectory
add_subdirectory(${MBED_OS_DIR} mbed-os)

# Define your project
project(MyMbedProject) # Replace with your project name

# Collect all source files in 'source' directory
file(GLOB_RECURSE PROJECT_SOURCES "source/*.cpp" "source/*.c")

# Collect all header files in 'header' directory (optional for compilation, but helpful for IDEs)
file(GLOB_RECURSE PROJECT_HEADERS "header/*.h" "header/*.hpp")

# Add the executable target
add_executable(${PROJECT_NAME} main.cpp ${PROJECT_SOURCES} ${PROJECT_HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        "header"
)

# Link against mbed-os
target_link_libraries(${PROJECT_NAME} PRIVATE mbed-os)

# Set up post-build actions (e.g., binary creation, code upload)
mbed_set_post_build(${PROJECT_NAME})

# Finalize the build (must be last)
mbed_finalize_build()
