cmake_minimum_required(VERSION 3.15)

# Set project name and version
project(ProjTest VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define source files
set(APP_FILE
    src/main.cpp
)

set(SOURCE_FILES
    src/logger/logger.cpp
    src/mqtt/mqtt.cpp
)

set(TEST_FILES
    src/logger/logger_test.cpp
    src/mqtt/mqtt_test.cpp
    test/test.cpp
)

# Define include directories variable
set(inc 
    src/mqtt
    src/logger
)

# Create include flags
set(INC_FLAGS "")
foreach(dir ${inc})
    list(APPEND INC_FLAGS -I${CMAKE_SOURCE_DIR}/${dir})
endforeach()

# Define output directories
set(OUT_DIR "${CMAKE_BINARY_DIR}/app")
set(TEST_DIR "${CMAKE_BINARY_DIR}/test")
set(COV_DIR "${TEST_DIR}/coverage_html")

# Create the output directory before building
file(MAKE_DIRECTORY ${TEST_DIR})
file(MAKE_DIRECTORY ${OUT_DIR})

# Set general flags (no coverage flags here)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Create the test executable
add_executable(utest ${SOURCE_FILES} ${TEST_FILES})
add_executable(app ${SOURCE_FILES} ${APP_FILE})

# Link Google Test
target_link_libraries(utest gtest gtest_main)

# Link libraries
set(LIB_FLAGS -L/usr/local/lib -lpaho-mqttpp3 -lpaho-mqtt3as -pthread)

# Add custom target to build the application binary
add_custom_target(buildapp
    COMMAND clang++ -std=c++20 ${LIB_FLAGS} ${INC_FLAGS} ${SOURCE_FILES} ${APP_FILE} -o ${OUT_DIR}/app
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Building the app binary"
)

# Add custom target to build the test binary with coverage flags
add_custom_target(buildtest
    COMMAND clang++ -std=c++20 -fprofile-instr-generate -fcoverage-mapping ${LIB_FLAGS} ${INC_FLAGS} ${SOURCE_FILES} ${TEST_FILES} -lgtest -lgtest_main -o ${TEST_DIR}/utest
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Building the test binary"
)

# Add custom target to run unit tests and collect coverage data
add_custom_target(coverage
    COMMAND LLVM_PROFILE_FILE=${TEST_DIR}/utest.profraw ${TEST_DIR}/utest
    COMMAND /usr/lib/llvm-17/bin/llvm-profdata merge -sparse ${TEST_DIR}/utest.profraw -o ${TEST_DIR}/utest.profdata
    COMMAND /usr/lib/llvm-17/bin/llvm-cov show ${TEST_DIR}/utest -instr-profile=${TEST_DIR}/utest.profdata -format=html -output-dir=${COV_DIR}
    COMMAND xdg-open ${COV_DIR}/index.html
    DEPENDS buildtest
    COMMENT "Running tests, collecting coverage, and generating report"
)

# Add custom target to run unit tests and collect coverage data
add_custom_target(buildall
    DEPENDS buildapp coverage
    COMMENT "Build all"
)


# Add target to clean the build and coverage files
add_custom_target(cleanall
    COMMAND rm -rf *
    COMMENT "Cleaning up build and coverage files"
)
