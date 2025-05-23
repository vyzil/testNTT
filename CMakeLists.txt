cmake_minimum_required(VERSION 3.30)
project(testNTT)

# 1. MULTICORE Option
option(MULTICORE "Enable parallelized execution, using OpenMP" ON)

if(MULTICORE)
    add_definitions(-DMULTICORE)
endif()

set(MULTICORE_OPTION "")
if(MULTICORE)
    set(MULTICORE_OPTION "-DMULTICORE=ON")
endif()

# 2. Set Directory Path
set(LIBFQFFT_DIR ${CMAKE_SOURCE_DIR}/depends/libfqfft)
set(LIBFF_DIR ${LIBFQFFT_DIR}/depends/libff)
set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/build/install)

# 3. LIBRARY
## libfqfft
if(NOT EXISTS ${LIBFQFFT_DIR}/.git)
    message(STATUS "Initializing and updating submodules...")
    execute_process(COMMAND git submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

### Build & Install libfqfft with MULTICORE
execute_process(
    COMMAND git submodule init
    WORKING_DIRECTORY ${LIBFQFFT_DIR}
)
execute_process(
    COMMAND git submodule update
    WORKING_DIRECTORY ${LIBFQFFT_DIR}
)
execute_process(
    COMMAND ${CMAKE_COMMAND} -S ${LIBFQFFT_DIR} -B ${LIBFQFFT_DIR}/build -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} ${MULTICORE_OPTION}
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${LIBFQFFT_DIR}/build --target install
)
execute_process(
    COMMAND make install
    WORKING_DIRECTORY ${LIBFQFFT_DIR}/build
)

# 4. OpenMP 설정 추가
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    if(MULTICORE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
    endif()
endif()

# 5. Target Build
## Include Directories
include_directories(${LIBFQFFT_DIR})
include_directories(${LIBFF_DIR})

## Link Directories
link_directories(${INSTALL_DIR}/lib)
find_library(GMP_LIB gmp REQUIRED)
find_library(GMPXX_LIB gmpxx REQUIRED)
find_library(PROCPS_LIB procps REQUIRED)

## Set Targets
file(GLOB POLYNOMIAL_MULTIPLICATION_SRC "src/polynomial_multiplication.cpp" "src/utils.cpp")
add_executable(polynomial_multiplication ${POLYNOMIAL_MULTIPLICATION_SRC})
target_link_libraries(polynomial_multiplication PRIVATE ${INSTALL_DIR}/lib/libff.a ${GMP_LIB} ${GMPXX_LIB} ${PROCPS_LIB} OpenMP::OpenMP_CXX)

file(GLOB GENERATE_INPUT_SRC "src/generate_input.cpp" "src/utils.cpp")
add_executable(generate_input ${GENERATE_INPUT_SRC})
target_link_libraries(generate_input PRIVATE ${INSTALL_DIR}/lib/libff.a ${GMP_LIB} ${GMPXX_LIB} ${PROCPS_LIB} OpenMP::OpenMP_CXX)

file(GLOB NTT_TEST_SRC "src/ntt_test.cpp" "src/utils.cpp")
add_executable(ntt_test ${NTT_TEST_SRC})
target_link_libraries(ntt_test PRIVATE ${INSTALL_DIR}/lib/libff.a ${GMP_LIB} ${GMPXX_LIB} ${PROCPS_LIB} OpenMP::OpenMP_CXX)

# 6. ETC
## Data Dir
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/data")

message(STATUS "MULTICORE_OPTION: ${MULTICORE_OPTION}")
