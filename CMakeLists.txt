cmake_minimum_required(VERSION 3.30)
project(testNTT)

# 1. Set Directory Path
set(LIBFQFFT_DIR ${CMAKE_SOURCE_DIR}/depends/libfqfft)
set(LIBFF_DIR ${LIBFQFFT_DIR}/depends/libff)
set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/depends/install)

# 2. LIBRARY
## libfqfft
### Clone
if(NOT EXISTS ${LIBFQFFT_DIR})
    message(STATUS "Cloning libfqfft into ${LIBFQFFT_DIR}")
    execute_process(
        COMMAND git clone https://github.com/vyzil/libfqfft.git ${LIBFQFFT_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/depends
    )
endif()

### Build & Install libfqfft
execute_process(
    COMMAND git submodule init
    WORKING_DIRECTORY ${LIBFQFFT_DIR}
)
execute_process(
    COMMAND git submodule update
    WORKING_DIRECTORY ${LIBFQFFT_DIR}
)
execute_process(
    COMMAND ${CMAKE_COMMAND} -S ${LIBFQFFT_DIR} -B ${LIBFQFFT_DIR}/build -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${LIBFQFFT_DIR}/build --target install
)
execute_process(
    COMMAND make install
    WORKING_DIRECTORY ${LIBFQFFT_DIR}/build
)
# 3. Target Build
## Include Directories
include_directories(${LIBFQFFT_DIR})
include_directories(${LIBFF_DIR})

## Link Directories
link_directories(${INSTALL_DIR}/lib)
find_library(GMP_LIB gmp REQUIRED)
find_library(GMPXX_LIB gmpxx REQUIRED)
find_library(PROCPS_LIB procps REQUIRED)

## Set Targets
file(GLOB TEST_NTT_SRC "src/test_ntt.cpp" "src/utils.cpp")
add_executable(testNTT ${TEST_NTT_SRC})

target_link_libraries(testNTT PRIVATE ${INSTALL_DIR}/lib/libff.a ${GMP_LIB} ${GMPXX_LIB} ${PROCPS_LIB})

file(GLOB GENERATE_INPUT_SRC "src/generate_input.cpp" "src/utils.cpp")
add_executable(generate_input ${GENERATE_INPUT_SRC})
target_link_libraries(generate_input PRIVATE ${INSTALL_DIR}/lib/libff.a ${GMP_LIB} ${GMPXX_LIB} ${PROCPS_LIB})

# 4. ETC
## Data Dir
add_custom_command(TARGET testNTT POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:testNTT>/data"
    COMMENT "Creating data directory in build folder"
)
