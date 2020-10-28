cmake_minimum_required(VERSION 3.18)
project(lab12MC)

set(CMAKE_CXX_STANDARD 14)
find_package(MPI REQUIRED)

add_executable(lab12MC main.cpp utils.cpp utils.h)
message(STATUS "Run: ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS} EXECUTABLE ${MPIEXEC_POSTFLAGS} ARGS")
target_link_libraries(lab12MC PUBLIC MPI::MPI_CXX)
