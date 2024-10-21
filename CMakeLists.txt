cmake_minimum_required(VERSION 3.30)
project(testNTT)

# libfqfft 라이브러리 경로 설정 (depends 내부에 있는 libfqfft)
set(LIBFQFFT_DIR ${CMAKE_SOURCE_DIR}/depends/libfqfft)
set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/depends/libfqfft/install)  # 설치 경로 설정

# libfqfft가 없으면 clone (필요하면 강제로 clone)
if(NOT EXISTS ${LIBFQFFT_DIR})
    message(STATUS "Cloning libfqfft into ${LIBFQFFT_DIR}")
    execute_process(
        COMMAND git clone --recursive https://github.com/scipr-lab/libfqfft.git ${LIBFQFFT_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/depends
    )
endif()

# libfqfft 빌드 및 설치
execute_process(
    COMMAND ${CMAKE_COMMAND} -S ${LIBFQFFT_DIR} -B ${LIBFQFFT_DIR}/build -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${LIBFQFFT_DIR}/build --target install
)

# 타겟 설정
add_executable(testNTT main.cpp)

# libfqfft 라이브러리와 헤더 경로 추가
include_directories(${INSTALL_DIR}/include)
target_link_libraries(testNTT PRIVATE ${INSTALL_DIR}/lib/libff.a)
