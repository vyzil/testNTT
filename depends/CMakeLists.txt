# depends/CMakeLists.txt
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

# execute_process(
#     COMMAND ${CMAKE_COMMAND} --build ${LIBFQFFT_DIR}/build --target install
# )
# execute_process(
#     COMMAND make install
#     WORKING_DIRECTORY ${LIBFQFFT_DIR}/build
# )

# libfqfft 빌드 설정
add_subdirectory(libfqfft)

# install 경로 지정
install(DIRECTORY ${LIBFQFFT_DIR}/include/ DESTINATION ${INSTALL_DIR}/include)
install(DIRECTORY ${LIBFQFFT_DIR}/lib/ DESTINATION ${INSTALL_DIR}/lib)
